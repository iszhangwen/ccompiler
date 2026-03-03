#pragma once


#include <string>
#include "usedef.h"

class Instruction;
class Function;
class Module;

class BasicBlock : public Value
{
public:
    template<typename T> using PtrList = std::list<std::shared_ptr<T>>;
    static std::shared_ptr<BasicBlock> create(Module* m, Function* func, const std::string &name) {
        return std::make_shared<BasicBlock>(m, func, name);
    }

    // 所属函数块
    void setModule(Module* m) {m_module = m;}
    Module* getModule() {return m_module;}
    void setParent(Function* parent) {m_function = parent;}
    Function* getParent() {return m_function;}

    // 指令管理
    void addInst(std::shared_ptr<Instruction> ins)  {m_instructions.push_back(ins);}
    PtrList<Instruction> getInsts() {return m_instructions;}
    bool isEmpty() const {return m_instructions.empty();}
    bool isTerminator() const;

    // 前驱子与后继子
    void addPredecessor(BasicBlock* bb) {m_predecessor.push_back(bb);}
    std::list<BasicBlock*> getPredecessor() {return m_predecessor;}
    void addSuccessor(BasicBlock* bb) {m_successor.push_back(bb);}
    std::list<BasicBlock*> getSuccessor() {return m_successor;}

private:
    BasicBlock(Module *m, Function *fun, const std::string &name)
    : Value(QualType(), name), m_module(m), m_function(fun){}
    
    // 模块与函数
    Module* m_module;
    Function* m_function;
    // 指令集合
    PtrList<Instruction> m_instructions;
    // 前驱子与后继子
    std::list<BasicBlock*> m_predecessor;
    std::list<BasicBlock*> m_successor;
};