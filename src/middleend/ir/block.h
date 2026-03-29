#pragma once

#include <string>
#include "usedef.h"

class Instruction;
class Function;
class Module;

class Argument;

class ReturnInst;
class BranchInst;

class AllocaInst;
class LoadInst;
class StoreInst;

class BinaryInst;

class BasicBlock : public Value
{
public:
    using InsIterator = std::list<Instruction*>::iterator;
    explicit BasicBlock(Function *fun, const std::string &name);
    ~BasicBlock();

    // 所属函数块
    void setParent(Function* parent) {m_parent = parent;}
    Function* getParent() {return m_parent;}

    // 指令管理
    void addAllocaInst(AllocaInst* ins);
    void removeAllocInst(AllocaInst* ins);
    void addInst(Instruction* ins);
    void removeInst(Instruction* ins);
    std::list<Instruction*> getInsts();
    bool isEmpty() const;
    bool isTerminator() const;

    // 前驱子与后继子
    void addPredecessor(BasicBlock* bb) {m_predecessor.push_back(bb);}
    std::list<BasicBlock*> getPredecessors() {return m_predecessor;}
    void addSuccessor(BasicBlock* bb) {m_successor.push_back(bb);}
    std::list<BasicBlock*> getSuccessors() {return m_successor;}

    // 支配树idom: 直接支配者
    void setIdom(BasicBlock* ptr) {m_idom = ptr;}
    BasicBlock* getIdom() {return m_idom;}

    // 支配树：获取该节点支配的节点
    void addDomChildren(BasicBlock* ptr) {m_domChildren.push_back(ptr);}
    std::list<BasicBlock*> getDomChildren() {return m_domChildren;}

    // 支配前沿
    void addDomFrontier(BasicBlock* ptr) {
        for (auto& node : m_domFrontier) {
            if (ptr == node) return;
        }
        m_domFrontier.push_back(ptr);
    }
    std::list<BasicBlock*> getDomFrontier() {return m_domFrontier;}
    void clearDomFrontier() {m_domFrontier.clear();}

    // @brief: temrminator指令
    ReturnInst* createReturn(QualType, Value*);
    BranchInst* createBr(Value* cond, BasicBlock* ifThen, BasicBlock* ifElse = nullptr);
    BranchInst* createBr(BasicBlock* dest);
    // @brief: 内存操作指令
    AllocaInst* createAlloca(QualType);
    LoadInst* createLoad(Value*);
    StoreInst* createStore(Value*, Value*);
    // @brief: binary指令
    BinaryInst* createBinary(int opc, Value* r, Value* l);

    // @brief: 打印基本块IR
    void toStringPrint();

private:
    // 所属函数
    Function* m_parent;
    // 指令集合: alloca指令和其他指令
    std::list<AllocaInst*> m_allocaInstructions;
    std::list<Instruction*> m_instructions;
    // 前驱子与后继子
    std::list<BasicBlock*> m_predecessor;
    std::list<BasicBlock*> m_successor;
    // 基本块的直接支配者
    BasicBlock* m_idom;
    // 基本块的支配前沿
    // 所有最近不能被X支配的节点
    std::list<BasicBlock*> m_domFrontier;
    // 该基本块直接支配的基本块
    std::list<BasicBlock*> m_domChildren;
};
