#include "irbuilder.h"
#include "decl.h"
#include "stmt.h"
#include "expr.h"
#include "symbol.h"

#include "module.h"
#include "function.h"
#include "block.h"
#include "instruction.h"
#include "inst/baseinst.h"
#include "inst/meminst.h"
#include "inst/binaryinst.h"
#include "inst/terminator.h"
#include "inst/otherinst.h"
#include "values/constant.h"
#include "values/argument.h"
#include "values/globalvariable.h"

using namespace ccompiler;

// 前向声明辅助函数
static bool isAddressValue(Value* val);
static Value* ensureRValue(BasicBlock* block, Value* val);

IRBuilder::IRBuilder() 
{
    m_astCtx.ir = nullptr;
    m_astCtx.symbol = nullptr;
    m_module = nullptr;
    m_curFuntion = nullptr;
}

/*-----------------------Declarations node----------------------------------*/
std::any IRBuilder::visit(TranslationUnitDecl* node)
{
    m_module = Arena::make<Module>();
    // 设置当前的函数为空
    m_module->setCurFunction(nullptr);
    for (auto decl : node->getDecls()) {
        if (decl == nullptr)
            continue;
        decl->accept(this);
    }
    return makeAny<Value>();
}

std::any IRBuilder::visit(LabelDecl* ld)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(ValueDecl* vd)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(DeclaratorDecl* dd)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(VarDecl* var)
{
    // 基本属性
    auto type = var->getType();
    auto name = var->getName();
    Value* varAddr = nullptr;
    Value* varVal = nullptr;

    // 解析初始化表达式
    auto valExpr = var->getInitExpr();
    if (valExpr) {
        auto valAny = valExpr->accept(this);
        varVal = anyPtr<Value>(valAny);
    }

    // 解析变量声明：若是全局变量
    if (var->getScope()->isFileScope()) {
        varAddr = m_module->createGlobalVar(type, name);
        m_module->addGlobalDeclAddr(var, varAddr);
    } 
    // 解析变量声明：若是局部变量
    else {
        auto func = m_module->getCurFunction();
        assert(func && "current function is nullptr!");
        // 在函数上分配栈区
        varAddr = func->getEntryBlock()->createAlloca(var->getType());
        // 将变量添加到函数
        func->addLocalDeclAddr(var, varAddr);
        // 将值store到变量地址
        auto curBB = func->getInsertBlock();
        if (curBB && varVal) {
            curBB->createStore(varVal, varAddr);
        }
    }

    // 返回ssa变量地址
    return makeAny<Value>(varAddr);
}
std::any IRBuilder::visit(ParmVarDecl* p)
{
    // 获取当前的插入函数
    auto func = m_module->getCurFunction();
    assert(func && "current function is nullptr!");

    // 获取当前的插入点
    auto curBlock = func->getInsertBlock();

    // 生成参数值对象
    auto type = p->getType();
    auto name = p->getName();
    auto index = func->getArguments().size();
    auto val = Arena::make<Argument>(type, name, index, func, curBlock);

    // 给参数值分配参数地址
    auto addr = func->getEntryBlock()->createAlloca(type); 

    // 将参数值写入到参数地址
    curBlock->createStore(val, addr);

    // 返回参数值
    return makeAny<Value>(val);
}
std::any IRBuilder::visit(FunctionDecl* node)
{   
    // 创建函数解析
    auto func = startFunction(node);
    // 解析函数体
    node->getBody()->accept(this);
    // 结束解析
    endFunction(func);
    return makeAny<Value>();
}
std::any IRBuilder::visit(FieldDecl* fd)
{
    return makeAny<Value>();
}
std::any  IRBuilder::visit(EnumConstantDecl* ecd)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(TypedefDecl* tnd)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(EnumDecl* ed)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(RecordDecl* rd)
{
    return makeAny<Value>();
}

/*-----------------------statemnts node----------------------------------*/
/*
@brief: 
*/
std::any IRBuilder::visit(LabelStmt* node)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(CaseStmt* node)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(DefaultStmt* node)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(CompoundStmt* node)
{
    for (auto stmt : node->getStmts()) {
        if (stmt == nullptr)
            continue;
        stmt->accept(this);
    }
    return makeAny<Value>();
}
std::any IRBuilder::visit(DeclStmt* node)
{
    auto decl = node->getDecl();
    assert(node && "DeclStmt empty!");
    return decl->accept(this);
}

std::any IRBuilder::visit(ExprStmt* node)
{
    auto decl = node->getExpr();
    assert(node && "ExprStmt empty!");
    return decl->accept(this);
}
std::any IRBuilder::visit(IfStmt* node)
{
    // 获取当前函数
    auto func = m_module->getCurFunction();
    assert(func && "not current function");

    // 创建 if.then if.else if.end块
    BasicBlock* ifthen = func->createAndInsertBlock("if.then");
    BasicBlock* ifend = func->createAndInsertBlock("if.end");
    BasicBlock* ifelse = ifend;
    if (node->getElse()) {
       ifelse = func->createAndInsertBlock("if.else");
    }

    auto condEx = node->getCond();
    auto thEx = node->getThen();
    auto elEx = node->getElse();
    // 解析条件表达式：返回的是值类型
    auto val = anyPtr<Value>(condEx->accept(this));
    assert(val && "condition is empty!");

    // 根据值类型进行跳转，插入br指令
    auto curBlock = func->getInsertBlock();
    curBlock->createBr(val, ifthen, ifelse);

    // 解析then块的指令
    func->emitBlock(ifthen);
    thEx->accept(this);
    // 解析完成后插入无条件br指令并设置跳转目标是end块，
    func->emitBranch(ifend);

    if (elEx) {
        // 解析else块的指令
        func->emitBlock(ifelse);
        elEx->accept(this);
        // 解析完成后插入无条件br指令并设置跳转目标是end块，
        func->emitBranch(ifend);
    }

    // @brief: 解析完成后，程序进入到end块
    func->emitBlock(ifend);
    return makeAny<Value>();
}
std::any IRBuilder::visit(SwitchStmt* ptr)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(WhileStmt* node)
{
    // 获取当前函数
    auto func = m_module->getCurFunction();
    assert(func && "not current function");

    // 创建循环基本块：循环头是单独的block
    auto loopHeader = func->createAndInsertBlock("while.cond");
    auto exit = func->createAndInsertBlock("while.exit");
    auto body = func->createAndInsertBlock("while.body");

    // 进入循环头基本块
    func->emitBlock(loopHeader);
    // 计算循环条件表达式的值
    auto valAny = node->getCond()->accept(this);
    auto val = anyPtr<Value>(valAny);
    // 根据计算的条件表达式值插入跳转指令
    loopHeader->createBr(val, body, exit);

    // 循环体执行开始前记录BC栈
    func->pushBCStack(exit, loopHeader);
    // 进入循环体
    func->emitBlock(body);
    // 执行循环语句
    node->getBody()->accept(this);
    // 循环体执行结束回到loopheader
    body->createBr(loopHeader);
    // 循环体执行完成后弹出BC栈
    func->popBCStack();

    // 循环结束控制流退出到exit块
    func->emitBlock(exit);
    return makeAny<Value>();
}
std::any IRBuilder::visit(DoStmt* ds)
{
    auto func = m_module->getCurFunction();
    assert(func && "not current function");

    // 创建循环体块和条件块
    auto body = func->createAndInsertBlock("do.body");
    auto cond = func->createAndInsertBlock("do.cond");
    auto exit = func->createAndInsertBlock("do.exit");

    // 无条件跳转到循环体
    auto curBlock = func->getInsertBlock();
    if (curBlock) {
        curBlock->createBr(body);
    }

    // 进入循环体
    func->pushBCStack(exit, cond);
    func->emitBlock(body);
    ds->getBody()->accept(this);
    // 循环体结束后跳转到条件块
    func->getInsertBlock()->createBr(cond);

    // 条件块：计算条件表达式
    func->emitBlock(cond);
    auto condVal = anyPtr<Value>(ds->getCond()->accept(this));
    condVal = ensureRValue(func->getInsertBlock(), condVal);
    cond->createBr(condVal, body, exit);

    func->popBCStack();

    // 退出循环
    func->emitBlock(exit);
    return makeAny<Value>();
}

std::any IRBuilder::visit(ForStmt* fs)
{
    auto func = m_module->getCurFunction();
    assert(func && "not current function");

    // 初始化表达式
    if (auto init = fs->getInit()) {
        init->accept(this);
    }

    // 创建循环头、循环体和退出块
    auto condBlock = func->createAndInsertBlock("for.cond");
    auto body = func->createAndInsertBlock("for.body");
    // If there's no update expr, for.update is the same as for.cond
    auto update = func->createAndInsertBlock("for.update");
    auto exit = func->createAndInsertBlock("for.exit");

    // 无条件跳转到条件块
    auto curBlock = func->getInsertBlock();
    if (curBlock) {
        curBlock->createBr(condBlock);
    }

    // 条件块
    func->emitBlock(condBlock);
    if (auto cond = fs->getCond()) {
        auto condVal = anyPtr<Value>(cond->accept(this));
        condVal = ensureRValue(func->getInsertBlock(), condVal);
        condBlock->createBr(condVal, body, exit);
    } else {
        // 无条件循环
        condBlock->createBr(body);
    }

    // 循环体
    func->pushBCStack(exit, update);
    func->emitBlock(body);
    fs->getBody()->accept(this);
    // 循环体结束后跳转到更新块
    func->getInsertBlock()->createBr(update);

    // 更新表达式
    func->emitBlock(update);
    if (auto upd = fs->getUpdate()) {
        upd->accept(this);
    }
    update->createBr(condBlock);

    func->popBCStack();

    // 退出循环
    func->emitBlock(exit);
    return makeAny<Value>();
}
std::any IRBuilder::visit(GotoStmt* gs)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(ContinueStmt* node)
{
    // 获取当前函数
    auto func = m_module->getCurFunction();
    assert(func && "not current function");

    // 获取当前的BC栈
    auto target = func->getBCBlock().second;
    assert(target && "ContinueStmt error");

    // 控制流转到当前loopheader块
    func->emitBlock(target);
    return makeAny<Value>();
}
std::any IRBuilder::visit(BreakStmt* ptr)
{
    // 获取当前函数
    auto func = m_module->getCurFunction();
    assert(func && "not current function");

    // 获取当前的BC栈
    auto target = func->getBCBlock().first;
    assert(target && "BreakStmt error");

    // 控制流转到当前exit块
    func->emitBlock(target);
    return makeAny<Value>();
}
std::any IRBuilder::visit(ReturnStmt* node)
{
    // 获取当前函数
    auto func = m_module->getCurFunction();
    assert(func && "not current function");

    if (auto expr = node->getRetExpr()) {
        // 计算返回值
        auto val = anyPtr<Value>(expr->accept(this));
        // 如果val是地址（DeclRefExpr返回的），加载为值
        auto curBlock = func->getInsertBlock();
        if (curBlock && isAddressValue(val)) {
            val = curBlock->createLoad(val);
        }
        // 获取返回值地址
        auto addr = func->getReturnAddr();
        // 将返回值写入到返回值地址
        func->ensureInsertPoint();
        func->getInsertBlock()->createStore(val, addr);
    }

    // 当遇到return关键字，控制流要转移到func.exit块
    func->emitBlock(func->getExitBlock());

    // 不确定程序接下来处于什么状态？因此清空当前插入点
    func->clearInsertPoint();
    return makeAny<Value>();
}

/*-----------------------expr node----------------------------------*/
std::any IRBuilder::visit(IntegerLiteral* node)
{
    auto val = ConstantInt::get(node->getType(), node->getValue());
    return makeAny<Value>(val);
}
std::any IRBuilder::visit(FloatingLiteral* node)
{
    auto val = ConstantFloat::get(node->getType(), node->getValue());
    return makeAny<Value>(val);
}
std::any IRBuilder::visit(CharacterLiteral* ptr)
{
    assert(ptr && "CharacterLiteral is nullptr");
    //ConstantChar::get(ptr->getValue());
    return makeAny<Value>();
}
std::any IRBuilder::visit(StringLiteral* ptr)
{
    assert(ptr && "StringLiteral is nullptr");
   // ConstantString::get(ptr->getValue());
    return makeAny<Value>();
}
// 辅助函数：判断Value是否是需要加载的地址类型
static bool isAddressValue(Value* val) {
    return dynamic_cast<AllocaInst*>(val) != nullptr
        || dynamic_cast<GlobalVariable*>(val) != nullptr;
}

// 辅助函数：确保Value是右值（如果是地址则加载）
static Value* ensureRValue(BasicBlock* block, Value* val) {
    if (isAddressValue(val)) {
        return block->createLoad(val);
    }
    return val;
}

std::any IRBuilder::visit(DeclRefExpr* node)
{
    // 获取当前函数
    auto func = m_module->getCurFunction();
    assert(func && "not current function");

    // 解析变量声明
    auto varDecl = node->getDecl();
    assert(varDecl && "varDecl error!");

    // 全局变量中查找变量地址
    if (varDecl->getScope()->isFileScope()) {
        auto addr = m_module->getGlobalDeclAddr(varDecl);
        assert(addr && "var not record in globalDeclMap");
        return makeAny<Value>(addr);
    }  

    // 函数局部变量：返回地址（由调用者决定是否加载为右值）
    auto addr = func->getLocalDeclAddr(varDecl);
    return makeAny<Value>(addr);
}
std::any IRBuilder::visit(ParenExpr* node)
{
    // 获取括号表达式
    auto expr = node->getSubExpr();
    if (expr == nullptr) {
        return nullptr;
    }

    // 返回表达式值
    return expr->accept(this);
}

std::any IRBuilder::visit(BinaryOpExpr* node)
{
    // 获取当前函数
    auto fun = m_module->getCurFunction();

    // 获取二元表达式的左式和右式
    auto lex = node->getLExpr();
    auto rex = node->getRExpr();
    if (lex == nullptr || rex == nullptr) {
        return nullptr;
    }

    // 分别计算左右子表达式的值
    Value* lval = anyPtr<Value>(lex->accept(this));
    Value* rval = anyPtr<Value>(rex->accept(this));

    // 获取当前插入点
    auto curBlock = fun->getInsertBlock();

    // 计算表达式运算结果
    Value* retVal = nullptr;

    // 对于赋值类操作：左值为地址，右值需要加载为值
    // 对于其他二元操作：左右值都需要加载为值（lvalue-to-rvalue转换）
    auto opc = node->getOpCode();
    if (opc == BinaryOpExpr::Assign_) {
        rval = ensureRValue(curBlock, rval);
        retVal = curBlock->createStore(rval, lval);
        return makeAny<Value>(retVal);
    } else if (opc != BinaryOpExpr::Comma) {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
    }

    switch (opc)
    {
    case BinaryOpExpr::Multiplication_:
        retVal = curBlock->createBinary(Instruction::Mul, lval, rval);
        break;
    case BinaryOpExpr::Division_:
        retVal = curBlock->createBinary(Instruction::Div, lval, rval);
        break;
    case BinaryOpExpr::Modulus_:
        retVal = curBlock->createBinary(Instruction::Mod, lval, rval);
        break;
    case BinaryOpExpr::Addition_:
        retVal = curBlock->createBinary(Instruction::Add, lval, rval);
        break;
    case BinaryOpExpr::Subtraction_:
        retVal = curBlock->createBinary(Instruction::Sub, lval, rval);
        break;
    case BinaryOpExpr::LShift_:
        retVal = curBlock->createBinary(Instruction::Shl, lval, rval);
        break;
    case BinaryOpExpr::RShift_:
        retVal = curBlock->createBinary(Instruction::Ashr, lval, rval);
        break;
    case BinaryOpExpr::Less_:
        retVal = curBlock->createBinary(Instruction::Lt, lval, rval);
        break;
    case BinaryOpExpr::Greater_:
        retVal = curBlock->createBinary(Instruction::Gt, lval, rval);
        break;
    case BinaryOpExpr::Less_Equal_:
        retVal = curBlock->createBinary(Instruction::Le, lval, rval);
        break;
    case BinaryOpExpr::Greater_Equal_:
        retVal = curBlock->createBinary(Instruction::Ge, lval, rval);
        break;
    case BinaryOpExpr::Equality_:
        retVal = curBlock->createBinary(Instruction::Eq, lval, rval);
        break;
    case BinaryOpExpr::Inequality_:
        retVal = curBlock->createBinary(Instruction::Ne, lval, rval);
        break;
    case BinaryOpExpr::BitWise_AND_:
        retVal = curBlock->createBinary(Instruction::And, lval, rval);
        break;
    case BinaryOpExpr::BitWise_XOR_:
        retVal = curBlock->createBinary(Instruction::Xor, lval, rval);
        break;
    case BinaryOpExpr::BitWise_OR_:
        retVal = curBlock->createBinary(Instruction::Or, lval, rval);
        break;
    case BinaryOpExpr::Logical_AND_:
        break;
    case BinaryOpExpr::Logical_OR_:
        break;
    case BinaryOpExpr::Mult_Assign_:
    {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
        auto val = curBlock->createBinary(Instruction::Mul, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::Div_Assign_:
    {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
        auto val = curBlock->createBinary(Instruction::Div, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::Mod_Assign_:
    {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
        auto val = curBlock->createBinary(Instruction::Mod, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::Add_Assign_:
    {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
        auto val = curBlock->createBinary(Instruction::Add, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::Sub_Assign_:
    {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
        auto val = curBlock->createBinary(Instruction::Sub, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::LShift_Assign_:
    {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
        auto val = curBlock->createBinary(Instruction::Shl, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::RShift_Assign_:
    {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
        auto val = curBlock->createBinary(Instruction::Ashr, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::BitWise_AND_Assign_:
    {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
        auto val = curBlock->createBinary(Instruction::And, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::BitWise_XOR_Assign_:
    {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
        auto val = curBlock->createBinary(Instruction::Xor, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::BitWise_OR_Assign_:
    {
        lval = ensureRValue(curBlock, lval);
        rval = ensureRValue(curBlock, rval);
        auto val = curBlock->createBinary(Instruction::Or, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::Comma:
        retVal = rval;
        break;
    default:
        assert("BinaryOpExpr error");
        break;
    }
    return makeAny<Value>(retVal);
}
std::any IRBuilder::visit(ConditionalExpr* node)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(CompoundLiteralExpr* node)
{
    auto expr = node->getInitExpr();
    if (expr) {
        return expr->accept(this);
    }
    return makeAny<Value>();
}
std::any IRBuilder::visit(CastExpr* node)
{
    // 类型转换：直接返回子表达式的值（隐式转换通常不改变位的表示）
    auto subExpr = node->getCastExpr();
    if (subExpr) {
        auto val = anyPtr<Value>(subExpr->accept(this));
        return makeAny<Value>(val);
    }
    return makeAny<Value>();
}

std::any IRBuilder::visit(ArraySubscriptExpr* node)
{
    return makeAny<Value>();
}

std::any IRBuilder::visit(CallExpr* node)
{
    auto func = m_module->getCurFunction();
    assert(func && "not current function");

    auto callee = node->getCallee();
    if (!callee) return makeAny<Value>();

    // 解析被调用者表达式（函数名等）
    auto calleeVal = anyPtr<Value>(callee->accept(this));
    assert(calleeVal && "callee is nullptr");

    // 获取函数名
    std::string calleeName;
    if (auto* declRef = dynamic_cast<DeclRefExpr*>(callee)) {
        calleeName = declRef->getDecl()->getName();
    }

    // 获取函数返回类型
    QualType retType;
    if (auto calleeType = callee->getType()) {
        if (auto ft = calleeType.as<FunctionType>()) {
            retType = ft->getRetType();
        }
    }

    // 创建CallInst
    auto curBlock = func->getInsertBlock();
    CallInst* callInst = Arena::make<CallInst>(retType, calleeVal, curBlock);

    // 处理参数
    for (auto param : node->getParams()) {
        auto paramVal = anyPtr<Value>(param->accept(this));
        if (paramVal) {
            paramVal = ensureRValue(curBlock, paramVal);
            callInst->addArg(paramVal);
        }
    }

    curBlock->addInst(callInst);
    return makeAny<Value>(callInst);
}

std::any IRBuilder::visit(MemberExpr* node)
{
    return makeAny<Value>();
}

std::any IRBuilder::visit(UnaryOpExpr* node)
{
    auto func = m_module->getCurFunction();
    assert(func && "not current function");

    auto subExpr = node->getSubExpr();
    if (!subExpr) return makeAny<Value>();

    auto curBlock = func->getInsertBlock();

    switch (node->getOpCode()) {
    case UnaryOpExpr::Addition_:
        // +x: 无操作，直接返回子表达式的值
        return subExpr->accept(this);

    case UnaryOpExpr::Subtraction_:
    {
        // -x: 取负，0 - x
        auto val = anyPtr<Value>(subExpr->accept(this));
        val = ensureRValue(curBlock, val);
        auto zero = ConstantInt::get(val->getType(), 0);
        auto neg = curBlock->createBinary(Instruction::Sub, zero, val);
        return makeAny<Value>(neg);
    }

    case UnaryOpExpr::Logical_NOT_:
    {
        // !x: 逻辑非
        auto val = anyPtr<Value>(subExpr->accept(this));
        val = ensureRValue(curBlock, val);
        // x == 0
        auto zero = ConstantInt::get(val->getType(), 0);
        auto cmp = curBlock->createBinary(Instruction::Eq, val, zero);
        return makeAny<Value>(cmp);
    }

    case UnaryOpExpr::BitWise_NOT_:
    {
        // ~x: 按位取反
        auto val = anyPtr<Value>(subExpr->accept(this));
        val = ensureRValue(curBlock, val);
        auto negOne = ConstantInt::get(val->getType(), -1);
        auto notVal = curBlock->createBinary(Instruction::Xor, val, negOne);
        return makeAny<Value>(notVal);
    }

    case UnaryOpExpr::Pre_Increment_:
    case UnaryOpExpr::Pre_Decrement_:
    {
        // ++x 或 --x: 先修改值再返回
        int delta = (node->getOpCode() == UnaryOpExpr::Pre_Increment_) ? 1 : -1;
        auto addr = anyPtr<Value>(subExpr->accept(this));
        auto one = ConstantInt::get(QualType(), delta);
        auto oldVal = curBlock->createLoad(addr);
        auto newVal = curBlock->createBinary(
            (delta > 0) ? Instruction::Add : Instruction::Sub, oldVal, one);
        curBlock->createStore(newVal, addr);
        return makeAny<Value>(newVal);
    }

    case UnaryOpExpr::Post_Increment_:
    case UnaryOpExpr::Post_Decrement_:
    {
        // x++ 或 x--: 先返回旧值再修改
        int delta = (node->getOpCode() == UnaryOpExpr::Post_Increment_) ? 1 : -1;
        auto addr = anyPtr<Value>(subExpr->accept(this));
        auto oldVal = curBlock->createLoad(addr);
        auto one = ConstantInt::get(QualType(), delta);
        auto newVal = curBlock->createBinary(
            (delta > 0) ? Instruction::Add : Instruction::Sub, oldVal, one);
        curBlock->createStore(newVal, addr);
        return makeAny<Value>(oldVal);
    }

    case UnaryOpExpr::BitWise_AND_:
        // &x: 取地址 - 返回变量的地址
        return subExpr->accept(this);

    case UnaryOpExpr::Multiplication_:
        // *x: 解引用 - 从地址加载值
    {
        auto addr = anyPtr<Value>(subExpr->accept(this));
        if (addr) {
            auto loaded = curBlock->createLoad(addr);
            return makeAny<Value>(loaded);
        }
        break;
    }

    default:
        break;
    }
    return makeAny<Value>();
}

void IRBuilder::run(AstCtx astIR)
{
    m_astCtx = astIR;
    visit(astIR.ir);
}

Function* IRBuilder::startFunction(FunctionDecl* node)
{
    // 不是定义则返回
    assert(node->isDefinition() && "function is not definition");
    // 嵌套函数定义不允许
    assert(!m_module->getCurFunction() && "Nested function definition");

    // 创建Fucntion模块,并设置为当前函数
    auto type = node->getType();
    auto newFunc = m_module->createFunctionIR(type, node->getName());
    if (newFunc == nullptr) {
        return nullptr;
    }
    m_module->setCurFunction(newFunc);
    m_curFuntion = newFunc;

    // 创建entry块
    auto entry = newFunc->createAndInsertBlock("entry");
    newFunc->setEntryBlock(entry);
    // 创建exit块
    auto exit = newFunc->createAndInsertBlock("exit");
    newFunc->setExitBlock(exit);

    // 进入entry基本块
    newFunc->emitBlock(entry);

    // 分配返回地址
    auto retType = type.as<FunctionType>()->getRetType();
    if (!retType->isVoidType()) {
        auto retAddr = entry->createAlloca(retType);
        newFunc->setReturnAddr(retAddr);
    }

    // 分配函数参数
    emitFunctionParams(node, newFunc);
    return newFunc;
}

void IRBuilder::emitFunctionParams(FunctionDecl *node, Function *func)
{
    for (auto& param : node->getParamVarDecls()) {
        if (param == nullptr)
            continue;
        auto val = anyPtr<Value, Argument>(param->accept(this));
        if (val) {
            func->addArgument(val);
        }
    }
}

void IRBuilder::endFunction(Function* func)
{
    // 检查控制流一致性
    // 生成函数返回的基本块，所有ret语句最终跳转到这里
    // 进入到exit块
    auto retBlock = func->getExitBlock();
    func->emitBlock(retBlock);

    // 将值从返回值地址读取出来,并插入ret val指令
    auto retAddr = func->getReturnAddr();
    auto retType = func->getRetType();
    if (retAddr) {
        auto retVal = retBlock->createLoad(retAddr);
        retBlock->createReturn(retType, retVal);
    } else {
        // void 函数：直接创建void返回
        retBlock->createReturn(retType, nullptr);
    }

    // 模块级清空当前函数
    m_module->setCurFunction(nullptr);
}

