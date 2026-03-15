#pragma once

#include <string>
#include "usedef.h"

class Instruction;
class Function;
class Module;

class BasicBlock : public Value
{
public:
    explicit BasicBlock(Module *m, Function *fun, const std::string &name);
    static BasicBlock* create(Module* m, Function* func, const std::string &name);

    // 所属函数块
    void setModule(Module* m) {m_module = m;}
    Module* getModule() {return m_module;}
    void setParent(Function* parent) {m_function = parent;}
    Function* getParent() {return m_function;}

    // 指令管理
    void addInst(Instruction* ins)  {m_instructions.push_back(ins);}
    std::list<Instruction*> getInsts() {return m_instructions;}
    bool isEmpty() const {return m_instructions.empty();}
    bool isTerminator() const;

    // 前驱子与后继子
    void addPredecessor(BasicBlock* bb) {m_predecessor.push_back(bb);}
    std::list<BasicBlock*> getPredecessors() {return m_predecessor;}
    void addSuccessor(BasicBlock* bb) {m_successor.push_back(bb);}
    std::list<BasicBlock*> getSuccessors() {return m_successor;}

    // 支配树idom
    void setIdom(BasicBlock* ptr) {m_idom = ptr;}
    BasicBlock* getIdom() {return m_idom;}

    // 支配前沿
    void addDomFrontier(BasicBlock* ptr) {
        for (auto& node : m_domFrontier) {
            if (ptr == node) return;
        }
        m_domFrontier.push_back(ptr);
    }
    std::list<BasicBlock*> getDomFrontier() {return m_domFrontier;}
    void clearDomFrontier() {m_domFrontier.clear();}

private:
    // 模块与函数
    Module* m_module;
    Function* m_function;
    // 指令集合
    std::list<Instruction*> m_instructions;
    // 前驱子与后继子
    std::list<BasicBlock*> m_predecessor;
    std::list<BasicBlock*> m_successor;
    // 基本块的直接支配者
    BasicBlock* m_idom;
    // 基本块的支配前沿
    // 所有最近不能被X支配的节点
    std::list<BasicBlock*> m_domFrontier;
};