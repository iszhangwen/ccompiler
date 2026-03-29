#include "function.h"
#include "block.h"
#include "instruction.h"

#include <iostream>
#include <algorithm>

Function::Function(QualType ty, const std::string& name, Module* parent)
    : GlobalValue(ty, name, parent)
{
    m_curBlock = nullptr;
}

Function::~Function()
{

}

// @brief：创建一个新块，并将程序控制权从当前块转移到新块
void Function::emitBlock(BasicBlock* target)
{
    // 当前块末尾创建分支指令
    emitBranch(target);
    // 将程序转移到当前块
    setInsertPoint(target);
}

// @brief: 当前块末尾插入br指令，br指令跳转目标target
void Function::emitBranch(BasicBlock* target)
{
    auto curBlock = getInsertBlock();
    if (!curBlock || curBlock->isTerminator()) {
        // 如果当前基本块为空或者已经没有了插入点 
        // 则什么也不做  
    } else {
        // 否则再当前块的末尾插入跳转指令
        // 跳转指令的跳转目标是新的基本块
        curBlock->createBr(target);
    }
    clearInsertPoint();
}

void Function::ensureInsertPoint()
{
    if (!hasInsertPoint()) {
        emitBlock(Arena::make<BasicBlock>(this, ""));
    }
}

void Function::pushBCStack(BasicBlock* B, BasicBlock* C)
{
    m_breakContStack.push(std::make_pair(B, C));
}

void Function::popBCStack()
{
    m_breakContStack.pop();
}

std::pair<BasicBlock*, BasicBlock*> Function::getBCBlock()
{
    if (m_breakContStack.empty())
        return std::make_pair(nullptr, nullptr);
    return m_breakContStack.top();
}

Value* Function::getLocalDeclAddr(NamedDecl* decl) {
    if (m_localDeclAddr.count(decl)) 
        return m_localDeclAddr[decl];
    return nullptr;
}
void Function::addLocalDeclAddr(NamedDecl* decl, Value* val) {
    if (m_localDeclAddr.count(decl)) 
        return;
    m_localDeclAddr[decl] = val;
}

BasicBlock* Function::createAndInsertBlock(const std::string& name)
{
    auto block = Arena::make<BasicBlock>(this, name);
    addBasicBlock(block);
    return block;
}

void Function::toStringPrint()
{
    for (auto block : getBasicBlocks()) {
        std::cout << "entry block: " << block->getName() << "\n";
        block->toStringPrint();
    }
}