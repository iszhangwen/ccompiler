#include "irbuilder.h"
#include "decl.h"
#include "stmt.h"
#include "expr.h"
#include "symbol.h"

#include "module.h"
#include "function.h"
#include "block.h"
#include "instruction.h"
#include "values/constant.h"
#include "values/argument.h"
#include "values/globalvariable.h"

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
        varVal = anyPtr<Value>(valExpr->accept(this));
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
    m_curFuntion->emitBlock(ifthen);
    thEx->accept(this);
    // 解析完成后插入无条件br指令并设置跳转目标是end块，
    m_curFuntion->emitBranch(ifend);

    if (elEx) {
        // 解析else块的指令
        m_curFuntion->emitBlock(ifelse);
        elEx->accept(this);
        // 解析完成后插入无条件br指令并设置跳转目标是end块，
        m_curFuntion->emitBranch(ifend);
    }

    // @brief: 解析完成后，程序进入到end块
    m_curFuntion->emitBlock(ifend);
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
    return makeAny<Value>();
}
std::any IRBuilder::visit(ForStmt* fs)
{
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
        return addr;
    }  

    // 函数局部变量查找变量地址
    auto val = func->getLocalDeclAddr(varDecl);
    return makeAny<Value>(val);
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
    switch (node->getOpCode())
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
        break;
    case BinaryOpExpr::RShift_:
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
    case BinaryOpExpr::Assign_:
        // 从右表达式获取值 存储到左式地址
        return curBlock->createStore(rval, lval);
    case BinaryOpExpr::Mult_Assign_:
    {
        auto val = curBlock->createBinary(Instruction::Add, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::Div_Assign_:
    {
        auto val = curBlock->createBinary(Instruction::Div, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::Mod_Assign_:
    {
        auto val = curBlock->createBinary(Instruction::Mod, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::Add_Assign_:
    {
        auto val = curBlock->createBinary(Instruction::Add, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::Sub_Assign_:
    {
        auto val = curBlock->createBinary(Instruction::Sub, lval, rval);
        retVal = curBlock->createStore(val, lval);
        break;
    }
    case BinaryOpExpr::LShift_Assign_:
        break;
    case BinaryOpExpr::RShift_Assign_:
        break;
    case BinaryOpExpr::BitWise_AND_Assign_:
        break;    
    case BinaryOpExpr::BitWise_XOR_Assign_:
        break;
    case BinaryOpExpr::BitWise_OR_Assign_:
        break;
    case BinaryOpExpr::Comma:
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
    return makeAny<Value>();
}
std::any IRBuilder::visit(ArraySubscriptExpr* node)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(CallExpr* node)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(MemberExpr* node)
{
    return makeAny<Value>();
}
std::any IRBuilder::visit(UnaryOpExpr* node)
{
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
    if (retAddr) {
        auto retVal = retBlock->createLoad(retAddr);
        retBlock->createReturn(retAddr->getType(), retVal);
    } else {
        retBlock->createReturn(retAddr->getType(), 0);
    }

    // 模块级清空当前函数
    m_module->setCurFunction(nullptr);
}

