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
    template<typename T> using PtrList = std::list<std::shared_ptr<T>>;
    Function(QualType ty, const std::string& name, Module* parent)
    : Value(ty, name), m_module(parent){}
    ~Function(){}

    // 模块
    Module* getParent() {return m_module;}
    void setParent(Module* parent) {m_module = parent;}

    // 基本块操作
    // 前驱子和后继子只有发生转移时才添加关系。
    PtrList<BasicBlock> getBasicBlocks() {return m_basicBlocks;}
    std::shared_ptr<BasicBlock> getInsertBlock() {return m_curBlock;}
    bool hasInsertPoint() const {return m_curBlock != nullptr;}
    void setInsertPoint(std::shared_ptr<BasicBlock> bb) {m_curBlock = bb;}
    void clearInsertPoint() {m_curBlock = nullptr;}

    // 函数参数
    void addArgument(std::shared_ptr<Argument> arg) {m_arguments.push_back(arg);}
    PtrList<Argument> getArguments() {return m_arguments;}

    // 构造辅助函数
    void emitBlock(std::shared_ptr<BasicBlock> bb, bool isFinihed=false);
    void emitBranch(std::shared_ptr<BasicBlock> bb);
    void ensureInsertPoint();

    // 函数指令管理
    void addInst(std::shared_ptr<Instruction> ins);

    // break和continue栈管理
    void pushBreakContinueStack(std::shared_ptr<BasicBlock>, std::shared_ptr<BasicBlock>);
    void popBreakContinueStack();
    std::pair<std::shared_ptr<BasicBlock>, std::shared_ptr<BasicBlock>> getBreakContinueStackBlock();

    // 函数返回值地址
    void setReturnAddr(std::shared_ptr<Instruction> ins) {m_returnAddr = ins;}
    std::shared_ptr<Instruction> getReturnAddr() {return m_returnAddr;}
    void setReturnBlock(std::shared_ptr<BasicBlock> bb) {m_returnBlock = bb;}
    std::shared_ptr<BasicBlock> getReturnBlock() {return m_returnBlock;}

    // 局部声明地址获取
    std::shared_ptr<Value> getLocalDeclAddr(NamedDecl* decl);
    void setLocalDeclAddr(NamedDecl* decl, std::shared_ptr<Value> val);

private:
    // 模块
    Module* m_module; 
    // 函数基本块
    std::shared_ptr<BasicBlock> m_curBlock;
    PtrList<BasicBlock> m_basicBlocks;
    // 函数参数
    PtrList<Argument> m_arguments; 
    // 函数返回值地址
    std::shared_ptr<Instruction> m_returnAddr;
    // 函数返回块地址
    std::shared_ptr<BasicBlock> m_returnBlock;
    // first: break栈，second: continue栈
    std::stack<std::pair<std::shared_ptr<BasicBlock>, std::shared_ptr<BasicBlock>>> m_breakContStack;
    // 局部变量和ssa ir的对应表
    std::unordered_map<NamedDecl*, std::shared_ptr<Value>> m_localDeclAddr;
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