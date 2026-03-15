#include "irbuilder.h"

#include <iostream>

#include "decl.h"
#include "stmt.h"
#include "expr.h"
#include "symbol.h"

#include "module.h"
#include "function.h"
#include "block.h"
#include "instruction.h"
#include "constant.h"
#include "value.h"

IRBuilder::IRBuilder() 
{
    m_module = nullptr;
    m_curFuntion = nullptr;
}

/*-----------------------Declarations node----------------------------------*/
void IRBuilder::visit(TranslationUnitDecl* ld)
{
    m_module = Arena::make<Module>("");
    // 遍历所有声明
    for (auto decl : ld->getDecls()) {
        decl->accept(this);
    }
    return std::any();
    std::any()
}
std::any IRBuilder::visit(LabelDecl* ld)
{
    return std::any();
}
std::any IRBuilder::visit(ValueDecl* vd)
{
    return std::any();
}
std::any IRBuilder::visit(DeclaratorDecl* dd)
{
    return std::any();
}
std::any IRBuilder::visit(VarDecl* var)
{
    auto ty = var->getQualType();
    // 判断是否是全局变量

    // 判断变量是否是常量
    
    // 初始值
    auto initExpr = var->getInitExpr();
    if (initExpr) {
        initExpr->accept(this);
    }
    return std::any();
}
std::any IRBuilder::visit(ParmVarDecl* p)
{
    auto curBB = m_curFuntion->getInsertBlock();
    auto index = m_curFuntion->getArguments().size();
    auto val = Arena::make<Argument>(p->getQualType(), p->getName(), index, m_curFuntion);
    // 函数栈上分配参数
    auto addr = Arena::make<AllocaInst>(p->getQualType(), curBB);
    m_curFuntion->addInst(addr);
    // 将参数值填充到栈内存
    auto store = Arena::make<StoreInst>(p->getQualType(), val, addr, curBB);
    m_curFuntion->addInst(store);
    return std::any();
}
std::any IRBuilder::visit(FunctionDecl* ptr)
{   
    m_curFuntion = Arena::make<Function>(ptr->getQualType(), ptr->getName(), m_module);
    m_module->addFunction(m_curFuntion);
    auto entry = BasicBlock::create(m_module, m_curFuntion, "entry");
    auto exit = BasicBlock::create(m_module, m_curFuntion, "ret");

    // 1. 创建起始块
    m_curFuntion->emitBlock(entry);
     m_curFuntion->setReturnBlock(exit);

    // 2. 分配函数返回地址: 返回值的分配必须在函数体执行前完成，因为它是函数逻辑的一部分，可能被多个地方访问，但不是必须的。
    auto retType = dynamic_cast<FunctionType*>(ptr->getQualType().getPtr())->getQualType();
    Instruction* retAddr = nullptr;
    if (!retType->isVoidType()) {
        retAddr = Arena::make<AllocaInst>(retType, entry);
        m_curFuntion->addInst(retAddr);
    }
    m_curFuntion->setReturnAddr(retAddr);

    // 3. 将参数插入到entry基本块中并分配地址
    for (auto param : ptr->getParmVarDeclList()) {
        param->accept(this);
    }
    // 4. 函数体指令生成
    ptr->getBody()->accept(this);

    // 5. 函数的返回块生成
    m_curFuntion->emitBlock(exit);

    Instruction* retVal = nullptr;
    if (!retType->isVoidType()) {
        retVal = Arena::make<LoadInst>(retType, retAddr);
        m_curFuntion->addInst(retVal);
    } 
    auto retInst = Arena::make<ReturnInst>(retVal);
    m_curFuntion->addInst(retInst);
    return std::any();
}
std::any IRBuilder::visit(FieldDecl* fd)
{
    return std::any();
}
std::any  IRBuilder::visit(EnumConstantDecl* ecd)
{
    return std::any();
}
std::any IRBuilder::visit(TypedefDecl* tnd)
{
    return std::any();
}
std::any IRBuilder::visit(EnumDecl* ed)
{
    return std::any();
}
std::any IRBuilder::visit(RecordDecl* rd)
{
    return std::any();
}


/*-----------------------statemnts node----------------------------------*/
/*
@brief: 
*/
std::any IRBuilder::visit(LabelStmt* ls)
{
    return std::any();
}
std::any IRBuilder::visit(CaseStmt* cs)
{
    return std::any();
}
std::any IRBuilder::visit(DefaultStmt* ds)
{
    return std::any();
}
std::any IRBuilder::visit(CompoundStmt* ptr)
{
    assert(ptr && "CompoundStmt is nullptr");
    for (auto stmt : ptr->getStmts()) {
        ptr->accept(this);
    }
    return std::any();
}
std::any IRBuilder::visit(DeclStmt* ptr)
{
    assert(ptr && "DeclStmt is nullptr");
    if (ptr->getDecl())
        ptr->getDecl()->accept(this);
    return std::any();
}
std::any IRBuilder::visit(ExprStmt* ptr)
{
    assert(ptr && "ExprStmt is nullptr");
    if (ptr->getExpr())
        ptr->getExpr()->accept(this);
    return std::any();
}
std::any IRBuilder::visit(IfStmt* ptr)
{
    assert(ptr && "ExprStmt is nullptr");
    auto ifThen = BasicBlock::create(m_module, m_curFuntion, "if.then");
    auto ifEnd = BasicBlock::create(m_module, m_curFuntion, "if.end");
    auto ifElse = ifEnd;
    if (ptr->getElse()) {
       ifElse = BasicBlock::create(m_module, m_curFuntion, "if.else");
    }
    // @brief: 条件指令生成 TODO
    

    // @brief: 解析then块，将当前基本块设置为then块，
    // 解析then块的指令，解析完成后插入br指令并设置跳转目标是end块，
    m_curFuntion->emitBlock(ifThen);
    ptr->getThen()->accept(this);
    m_curFuntion->emitBranch(ifEnd);

    // @brief: 解析else块
    if (ptr->getElse()) {
        m_curFuntion->emitBlock(ifElse);
        ptr->getElse()->accept(this);
        m_curFuntion->emitBranch(ifEnd);
    }

    // @brief: 解析完成后，程序进入到end块
    m_curFuntion->emitBlock(ifEnd, true);
    return std::any();
}
std::any IRBuilder::visit(SwitchStmt* ptr)
{
    return std::any();
}
std::any IRBuilder::visit(WhileStmt* ptr)
{
    assert(ptr && "WhileStmt、 is nullptr");
    // @brief: 循环头是单独的block
    auto loopHeader = BasicBlock::create(m_module, m_curFuntion, "while.cond");
    auto exit = BasicBlock::create(m_module, m_curFuntion, "while.exit");
    auto body = BasicBlock::create(m_module, m_curFuntion, "while.body");

    // 循环开始前必须要从loopHeader开始
    m_curFuntion->emitBlock(loopHeader);
    //@brief: 计算循环条件表达式的值 TODO
    Value* cond = nullptr;

    //@brief: 循环条件块插入br指令
    auto brInst = Arena::make<BranchInst>(loopHeader, cond, body, exit);

    //@brief: 循环体 针对break和continue存储块
    m_curFuntion->pushBreakContinueStack(exit, loopHeader);
    m_curFuntion->emitBlock(body);
    ptr->getBody()->accept(this);
    m_curFuntion->popBreakContinueStack();

    // body结束后需要回到loopHeader
    m_curFuntion->emitBranch(loopHeader);
    // 退出循环
    m_curFuntion->emitBlock(exit, true);
    return std::any();
}
std::any IRBuilder::visit(DoStmt* ds)
{
    return std::any();
}
std::any IRBuilder::visit(ForStmt* fs)
{
    return std::any();
}
std::any IRBuilder::visit(GotoStmt* gs)
{
    return std::any();
}
std::any IRBuilder::visit(ContinueStmt* ptr)
{
    assert(ptr && "ContinueStmt is nullptr");
    auto continueBlock = m_curFuntion->getBreakContinueStackBlock().second;
    m_curFuntion->emitBranch(continueBlock);
    return std::any();
}
std::any IRBuilder::visit(BreakStmt* ptr)
{
    assert(ptr && "BreakStmt is nullptr");
    auto breakBlock = m_curFuntion->getBreakContinueStackBlock().first;
    m_curFuntion->emitBranch(breakBlock);
    return std::any();
}
std::any IRBuilder::visit(ReturnStmt* ptr)
{
    assert(ptr && "ReturnStmt is nullptr");
    // 获取函数返回块
    if (!m_curFuntion->hasInsertPoint())
        return std::any();
    m_curFuntion->emitBranch(m_curFuntion->getReturnBlock());
    return std::any();
}

/*-----------------------expr node----------------------------------*/
std::any IRBuilder::visit(IntegerLiteral* ptr)
{
    assert(ptr && "IntegerLiteral is nullptr");
    //ConstantInt::get(ptr->getValue());
    return std::any();
}
std::any IRBuilder::visit(FloatingLiteral* ptr)
{
    assert(ptr && "FloatingLiteral is nullptr");
    //ConstantFloat::get(ptr->getValue());
    return std::any();
}
std::any IRBuilder::visit(CharacterLiteral* ptr)
{
    assert(ptr && "CharacterLiteral is nullptr");
    //ConstantChar::get(ptr->getValue());
    return std::any();
}
std::any IRBuilder::visit(StringLiteral* ptr)
{
    assert(ptr && "StringLiteral is nullptr");
   // ConstantString::get(ptr->getValue());
    return std::any();
}
std::any IRBuilder::visit(DeclRefExpr* ptr)
{
    assert(ptr && "DeclRefExpr is nullptr");
    auto var = ptr->getDecl();
    assert(var);
    // 查找到变量的ssa ir地址
    if (var && var->getScope()->isFileScope()) {
        auto addr = m_module->getGlobalDeclAddr(var);
        assert(addr && "var not record in globalDeclMap");
        return std::any(addr);
    }  
    auto addr = m_curFuntion->getLocalDeclAddr(var);
    assert(addr && "var not record in localDeclMap");
    return std::any(addr);
}
std::any IRBuilder::visit(ParenExpr* ptr)
{
    assert(ptr && "ParenExpr is nullptr");
    if (ptr->getSubExpr())
        ptr->getSubExpr()->accept(this);
    return std::any();
}
std::any IRBuilder::visit(BinaryOpExpr* ptr)
{
    assert(ptr && "BinaryOpExpr is nullptr");
    auto lex = ptr->getLExpr();
    auto rex = ptr->getRExpr();

    // 分别计算左右子表达式的值
    lex->accept(this);
    rex->accept(this);

    // 计算结果表达式类型
    // 暂时都当作整形计算
    switch (ptr->getOpCode())
    {
    case BinaryOpExpr::Multiplication_:
        break;
    case BinaryOpExpr::Division_:
        break;
    case BinaryOpExpr::Modulus_:
        break;    
    case BinaryOpExpr::Addition_:
        break;
    case BinaryOpExpr::Subtraction_:
        break;
    case BinaryOpExpr::LShift_:
        break;
    case BinaryOpExpr::RShift_:
        break;
    case BinaryOpExpr::Less_:
        break;
    case BinaryOpExpr::Greater_:
        break;    
    case BinaryOpExpr::Less_Equal_:
        break;
    case BinaryOpExpr::Greater_Equal_:
        break;
    case BinaryOpExpr::Equality_:
        break;
    case BinaryOpExpr::Inequality_:
        break;
    case BinaryOpExpr::BitWise_AND_:
        break;
    case BinaryOpExpr::BitWise_XOR_:
        break;    
    case BinaryOpExpr::BitWise_OR_:
        break;
    case BinaryOpExpr::Logical_AND_:
        break;
    case BinaryOpExpr::Logical_OR_:
        break;
    case BinaryOpExpr::Assign_:
        break;
    case BinaryOpExpr::Mult_Assign_:
        break;
    case BinaryOpExpr::Div_Assign_:
        break;    
    case BinaryOpExpr::Mod_Assign_:
        break;
    case BinaryOpExpr::Add_Assign_:
        break;
    case BinaryOpExpr::Sub_Assign_:
        break;
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
        break;
    }
    return std::any();
}
std::any IRBuilder::visit(ConditionalExpr* ce)
{
    return std::any();
}
std::any IRBuilder::visit(CompoundLiteralExpr* cle)
{
    return std::any();
}
std::any IRBuilder::visit(CastExpr* ce)
{
    return std::any();
}
std::any IRBuilder::visit(ArraySubscriptExpr* ase)
{
    return std::any();
}
std::any IRBuilder::visit(CallExpr* ce)
{
    return std::any();
}
std::any IRBuilder::visit(MemberExpr* me)
{
    return std::any();
}
std::any IRBuilder::visit(UnaryOpExpr* uoe)
{
    return std::any();
}


void IRBuilder::dump()
{
    for (auto func : m_module->getFunctions()) {
        std::cout << "start func: " << func->getName() << "\n";
        for (auto bb : func->getBasicBlocks()) {
            std::cout << "start block: " << bb->getName() << "\n";
            for (auto inst : bb->getInsts()) {
                auto num = inst->getNumOperands();
                std::cout << inst->getName() << " \n";
            }
        }
    }
}
