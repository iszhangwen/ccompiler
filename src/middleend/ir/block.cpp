#include "block.h"

#include "function.h"
#include "instruction.h"
#include "values/constant.h"

#include <iostream>

BasicBlock::BasicBlock(Function* fun, const std::string &name)
: Value(QualType(), name), m_parent(fun)
{

}

BasicBlock::~BasicBlock()
{

}

void BasicBlock::addAllocaInst(AllocaInst* ins) 
{
    m_allocaInstructions.push_back(ins);
}

void BasicBlock::removeAllocInst(AllocaInst* ins) 
{
    m_allocaInstructions.remove(ins);
}

void BasicBlock::addInst(Instruction* ins)  
{
    m_instructions.push_back(ins);
}

void BasicBlock::removeInst(Instruction* ins) 
{
    m_instructions.remove(ins);
}

std::list<Instruction*> BasicBlock::getInsts() 
{
    std::list<Instruction*> res;
    res.insert(res.end(), m_allocaInstructions.begin(), m_allocaInstructions.end());
    res.insert(res.end(), m_instructions.begin(), m_instructions.end());
    return res;
}

bool BasicBlock::isEmpty() const 
{
    return m_instructions.empty();
}

bool BasicBlock::isTerminator() const
{
    if (m_instructions.empty())
        return false;
    auto ins = m_instructions.back();
    if (ins == nullptr)
        return false;
    return ins->isTerminator();
}

AllocaInst* BasicBlock::createAlloca(QualType ty)
{
    AllocaInst* res = nullptr;
    // 如果不在函数中, 将alloca指令加入到当前块
    if (m_parent == nullptr) {
        if (res = Arena::make<AllocaInst>(ty, this)) {
            addAllocaInst(res);
        }
        return res;
    }
    // 如果在函数中， 将alloca指令加入到entry块
    auto entry = m_parent->getEntryBlock();
    if (res = Arena::make<AllocaInst>(ty, entry)) {
        // 搜索entry块的alloca区域
        entry->addAllocaInst(res);
    }
    return res;
}

ReturnInst* BasicBlock::createReturn(QualType ty, Value* val)
{
    ReturnInst* res = nullptr;
    if (ty->isVoidType()) {
        res = Arena::make<ReturnInst>(ty, this);
    } else {
        res = Arena::make<ReturnInst>(ty, val, this);
    }
    if (res) {
        addInst(res);
    }
    return res;
}

BranchInst* BasicBlock::createBr(Value* cond, BasicBlock* ifThen, BasicBlock* ifElse)
{
    BranchInst* res = nullptr;
    if (res = Arena::make<BranchInst>(this, cond, ifThen, ifElse)) {
        addInst(res);
    }
    return res;
}

BranchInst* BasicBlock::createBr(BasicBlock* dest)
{
    BranchInst* res = nullptr;
    if (res = Arena::make<BranchInst>(this, dest)) {
        addInst(res);
    }
    return res;
}

LoadInst* BasicBlock::createLoad(Value* val)
{
    LoadInst* res = nullptr;
    if (res = Arena::make<LoadInst>(val, this)) {
        addInst(res);
    }
    return res;
}

StoreInst* BasicBlock::createStore(Value* val, Value* addr)
{
    StoreInst* res = nullptr;
    if (res = Arena::make<StoreInst>(val, addr, this)) {
        addInst(res);
    }
    return res;
}

BinaryInst* BasicBlock::createBinary(int opc, Value* r, Value* l)
{
    BinaryInst* res = nullptr;
    auto op = static_cast<Instruction::OpCode>(opc);
    if (res = Arena::make<BinaryInst>(op, r, l, this)) {
        addInst(res);
    }
    return res;
}


void BasicBlock::toStringPrint()
{
    for (auto ins : getInsts()) {
        ins->toStringPrint();
    }
}
