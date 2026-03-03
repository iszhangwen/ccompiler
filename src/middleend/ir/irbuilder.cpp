#include "irbuilder.h"

#include <iostream>

#include "decl.h"
#include "stmt.h"
#include "expr.h"
#include "module.h"
#include "function.h"
#include "block.h"
#include "instruction.h"

IRBuilder::IRBuilder() 
{
    m_module = nullptr;
    m_curFuntion = nullptr;
    m_curBlock = nullptr;
}

/*-----------------------Declarations node----------------------------------*/
void IRBuilder::visit(TranslationUnitDecl* ld)
{
    m_module = std::make_shared<Module>("");
    // 遍历所有声明
    for (auto decl : ld->getDecls()) {
        decl->accept(this);
    }
}
void IRBuilder::visit(LabelDecl* ld)
{
    std::cout << " LabelDecl" << std::endl;
}
void IRBuilder::visit(ValueDecl* vd)
{
    std::cout << " ValueDecl" << std::endl;
}
void IRBuilder::visit(DeclaratorDecl* dd)
{
    std::cout << " DeclaratorDecl" << std::endl;
}
void IRBuilder::visit(VarDecl* var)
{
    auto ty = var->getQualType();
    // 判断是否是全局变量
    if (var->getScope()->isFileScope()) {

    }
    // 判断变量是否是常量
    
    // 初始值
    auto initExpr = var->getInitExpr();
    if (initExpr) {
        initExpr->accept(this);
    }
}
void IRBuilder::visit(ParmVarDecl* p)
{
    auto index = m_curFuntion->getArguments().size();
    auto val = std::make_shared<Argument>(p->getQualType(), p->getName(), index, m_curFuntion.get());
    // 函数栈上分配参数
    auto addr = std::make_shared<AllocaInst>(p->getQualType(), m_curBlock.get());
    m_curBlock->addInst(addr);
    // 将参数值填充到栈内存
    auto store = std::make_shared<StoreInst>(p->getQualType(), val, addr, m_curBlock.get());
    m_curBlock->addInst(store);
}
void IRBuilder::visit(FunctionDecl* fd)
{   
    // 创建函数对象
    m_curFuntion = std::make_shared<Function>(fd->getQualType(), fd->getName(), m_module.get());
    m_module->addFunction(m_curFuntion);
    // 1. 创建起始块
    auto entry = BasicBlock::create(m_module.get(), "entry", m_curFuntion.get());
    setInsertPoint(entry);
    m_curFuntion->addBasicBlock(entry);

    // 2. 分配函数返回地址: 返回值的分配必须在函数体执行前完成，因为它是函数逻辑的一部分，可能被多个地方访问，但不是必须的。
    auto retType = dynamic_cast<FunctionType*>(fd->getQualType().getPtr().get())->getQualType();
    std::shared_ptr<Instruction> retAddr = nullptr;
    if (!retType->isVoidType()) {
        retAddr = std::make_shared<AllocaInst>(retType, m_curBlock.get());
        m_curBlock->addInst(retAddr);
    }

    // 3. 将参数插入到entry基本块中并分配地址
    for (auto param : fd->getParmVarDeclList()) {
        param->accept(this);
    }
    // 4. 函数体指令生成
    fd->getBody()->accept(this);

    // 5. 函数的返回块生成
    auto exit = BasicBlock::create(m_module.get(), "ret", m_curFuntion.get());
    setInsertPoint(exit);
    m_curFuntion->addBasicBlock(exit);

    std::shared_ptr<Instruction> retVal = nullptr;
    if (!retType->isVoidType()) {
        retVal = std::make_shared<LoadInst>(retType, retAddr, m_curBlock.get());
        m_curBlock->addInst(retVal);
    } 
    auto retInst = std::make_shared<ReturnInst>(retVal, m_curBlock.get());
    m_curBlock->addInst(retInst);
}
void IRBuilder::visit(FieldDecl* fd)
{
    std::cout << " FieldDecl" << std::endl;
}
void IRBuilder::visit(EnumConstantDecl* ecd)
{
    std::cout << " EnumConstantDecl" << std::endl;
}
void IRBuilder::visit(TypedefDecl* tnd)
{
    std::cout << " TypedefDecl" << std::endl;
}
void IRBuilder::visit(EnumDecl* ed)
{
    std::cout << " EnumDecl" << std::endl;
}
void IRBuilder::visit(RecordDecl* rd)
{
    std::cout << " RecordDecl" << std::endl;
}

/*-----------------------expr node----------------------------------*/

void IRBuilder::visit(IntegerLiteral* c)
{
    std::cout << " IntegerLiteral" << std::endl;
}
void IRBuilder::visit(FloatingLiteral* c)
{
    std::cout << " FloatingLiteral" << std::endl;
}
void IRBuilder::visit(CharacterLiteral* c)
{
    std::cout << " CharacterLiteral" << std::endl;
}
void IRBuilder::visit(StringLiteral* c)
{
    std::cout << " StringLiteral" << std::endl;
}
void IRBuilder::visit(DeclRefExpr* dre)
{
    std::cout << " DeclRefExpr" << std::endl;
}
void IRBuilder::visit(ParenExpr* pe)
{
    std::cout << " ParenExpr" << std::endl;
}
void IRBuilder::visit(BinaryOpExpr* boe)
{
    std::cout << " BinaryOpExpr" << std::endl;
}
void IRBuilder::visit(ConditionalExpr* ce)
{
    std::cout << " ConditionalExpr" << std::endl;
}
void IRBuilder::visit(CompoundLiteralExpr* cle)
{
    std::cout << " CompoundLiteralExpr" << std::endl;
}
void IRBuilder::visit(CastExpr* ce)
{
    std::cout << " CastExpr" << std::endl;
}
void IRBuilder::visit(ArraySubscriptExpr* ase)
{
    std::cout << " ArraySubscriptExpr" << std::endl;
}
void IRBuilder::visit(CallExpr* ce)
{
    std::cout << " CallExpr" << std::endl;
}
void IRBuilder::visit(MemberExpr* me)
{
    std::cout << " MemberExpr" << std::endl;
}
void IRBuilder::visit(UnaryOpExpr* uoe)
{
    std::cout << " UnaryOpExpr" << std::endl;
}
/*-----------------------statemnts node----------------------------------*/
/*
Label stmt需要先创建一个
*/
void IRBuilder::visit(LabelStmt* ls)
{
    std::cout << " LabelStmt" << std::endl;
}
void IRBuilder::visit(CaseStmt* cs)
{
    std::cout << " CaseStmt" << std::endl;
}
void IRBuilder::visit(DefaultStmt* ds)
{
    std::cout << " DefaultStmt" << std::endl;
}
void IRBuilder::visit(CompoundStmt* cs)
{
    std::cout << " CompoundStmt" << std::endl;
}
void IRBuilder::visit(DeclStmt* ds)
{
    std::cout << " DeclStmt" << std::endl;
}
void IRBuilder::visit(ExprStmt* es)
{
    std::cout << " ExprStmt" << std::endl;
}
void IRBuilder::visit(IfStmt* is)
{
    std::cout << " IfStmt" << std::endl;
}
void IRBuilder::visit(SwitchStmt* ss)
{
    std::cout << " SwitchStmt" << std::endl;
}
void IRBuilder::visit(WhileStmt* ws)
{
    std::cout << " WhileStmt" << std::endl;
}
void IRBuilder::visit(DoStmt* ds)
{
    std::cout << " DoStmt" << std::endl;
}
void IRBuilder::visit(ForStmt* fs)
{
    std::cout << " ForStmt" << std::endl;
}
void IRBuilder::visit(GotoStmt* gs)
{
    std::cout << " GotoStmt" << std::endl;
}
void IRBuilder::visit(ContinueStmt* cs)
{
    std::cout << " ContinueStmt" << std::endl;
}
void IRBuilder::visit(BreakStmt* bs)
{
    std::cout << " BreakStmt" << std::endl;
}
void IRBuilder::visit(ReturnStmt* rs)
{
    std::cout << " ReturnStmt" << std::endl;
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

void IRBuilder::emitBlock(std::shared_ptr<BasicBlock> bb, bool isFinished)
{
    emitBranch(bb);
}

void IRBuilder::emitBranch(std::shared_ptr<BasicBlock> bb)
{

}