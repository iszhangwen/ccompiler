#pragma once

#include "usedef.h"
#include "module.h"
#include <vector>
#include <stack>
#include <unordered_map>

class BasicBlock;
class Argument;
class Instruction;

class Function : public Value
{
public:
    Function(QualType ty, const std::string& name, Module* parent)
    : Value(ty, name), m_module(parent){}
    ~Function(){}

    // 模块
    Module* getParent() {return m_module;}
    void setParent(Module* parent) {m_module = parent;}

    // 基本块操作
    // 前驱子和后继子只有发生转移时才添加关系。
    std::list<BasicBlock*> getBasicBlocks() {return m_basicBlocks;}
    BasicBlock* getInsertBlock() {return m_curBlock;}
    bool hasInsertPoint() const {return m_curBlock != nullptr;}
    void setInsertPoint(BasicBlock* bb) {m_curBlock = bb;}
    void clearInsertPoint() {m_curBlock = nullptr;}

    // 函数参数
    void addArgument(Argument* arg) {m_arguments.push_back(arg);}
    std::list<Argument*> getArguments() {return m_arguments;}

    // 构造辅助函数
    void emitBlock(BasicBlock* bb, bool isFinihed=false);
    void emitBranch(BasicBlock* bb);
    void ensureInsertPoint();

    // 函数指令管理
    void addInst(Instruction* ins);

    // break和continue栈管理
    void pushBreakContinueStack(BasicBlock*, BasicBlock*);
    void popBreakContinueStack();
    std::pair<BasicBlock*, BasicBlock*> getBreakContinueStackBlock();

    // 函数返回值地址
    void setReturnAddr(Instruction* ins) {m_returnAddr = ins;}
    Instruction* getReturnAddr() {return m_returnAddr;}
    void setReturnBlock(BasicBlock* bb) {m_returnBlock = bb;}
    BasicBlock* getReturnBlock() {return m_returnBlock;}

    // 局部声明地址获取
    Value* getLocalDeclAddr(NamedDecl* decl);
    void setLocalDeclAddr(NamedDecl* decl, Value* val);

    BasicBlock* getEntryBlock() {return m_entrylock;}
    void setEntryBlock(BasicBlock* bb) {m_entrylock = bb;}

private:
    // 模块
    Module* m_module; 
    // 函数基本块
    BasicBlock* m_curBlock;
    std::list<BasicBlock*> m_basicBlocks;
    // 函数参数
    std::list<Argument*> m_arguments; 
    // 函数返回值地址
    Instruction* m_returnAddr;
    // 函数返回块地址
    BasicBlock* m_returnBlock;
    // 函数入口块地址
    BasicBlock* m_entrylock;
    // first: break栈，second: continue栈
    std::stack<std::pair<BasicBlock*, BasicBlock*>> m_breakContStack;
    // 局部变量和ssa ir的对应表
    std::unordered_map<NamedDecl*, Value*> m_localDeclAddr;
};

class Argument : public Value
{
public:
    explicit Argument(QualType ty, const std::string& name, unsigned index, Function* fun = nullptr)
    : Value(ty, name), m_index(index), m_function(fun){}

    // 函数
    Function* getParent() {return m_function;}
    void setParent(Function* fun) {m_function = fun;}
    // 获取参数序号
    unsigned getIndex() const {return m_index;}
    void setIndex(unsigned index) {m_index = index;}

private:
    Function* m_function;
    unsigned int m_index;
};