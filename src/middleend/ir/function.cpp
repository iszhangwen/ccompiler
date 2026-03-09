#include "function.h"
#include "block.h"
#include "instruction.h"

// @brief：创建一个新块，并将程序控制权从当前块转移到新块
void Function::emitBlock(std::shared_ptr<BasicBlock> target, bool isFinihed)
{
    // 当前块末尾创建分支指令
    emitBranch(target);
    // 基本块插入到block中
    m_basicBlocks.push_back(target);
    // 将程序转移到当前块
    setInsertPoint(target);
}

// @brief: 当前块末尾插入br指令，br指令跳转目标target
void Function::emitBranch(std::shared_ptr<BasicBlock> target)
{
    auto curBlock = getInsertBlock();
    if (!curBlock || curBlock->isTerminator()) {
        // 如果当前基本块为空或者已经没有了插入点 
        // 则什么也不做  
    } else {
        // 否则再当前块的末尾插入跳转指令
        // 跳转指令的跳转目标是新的基本块
        m_curBlock->addInst(std::make_shared<BranchInst>(curBlock.get(), target.get()));
    }
    clearInsertPoint();
}

void Function::ensureInsertPoint()
{
    if (!hasInsertPoint()) {
        emitBlock(BasicBlock::create(m_module, this, ""));
    }
}

void Function::addInst(std::shared_ptr<Instruction> ins)
{
    auto curBlock = getInsertBlock();
    if (curBlock && ins) {
        ins->setParent(curBlock.get());
        curBlock->addInst(ins);
    }
}

void Function::pushBreakContinueStack(std::shared_ptr<BasicBlock> B, std::shared_ptr<BasicBlock> C)
{
    m_breakContStack.push(std::make_pair(B, C));
}

void Function::popBreakContinueStack()
{
    m_breakContStack.pop();
}

std::pair<std::shared_ptr<BasicBlock>, std::shared_ptr<BasicBlock>> Function::getBreakContinueStackBlock()
{
    return m_breakContStack.top();
}

std::shared_ptr<Value> Function::getLocalDeclAddr(NamedDecl* decl) {
    if (m_localDeclAddr.count(decl)) 
        return m_localDeclAddr[decl];
    return nullptr;
}
void Function::setLocalDeclAddr(NamedDecl* decl, std::shared_ptr<Value> val) {
    if (m_localDeclAddr.count(decl)) 
        return;
    m_localDeclAddr[decl] = val;
}