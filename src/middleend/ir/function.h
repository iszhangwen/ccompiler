#pragma once

#include "usedef.h"
#include "module.h"
#include <vector>

class BasicBlock;
class Argument;

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
    std::shared_ptr<BasicBlock> getInsertBlock() {
        if (m_curBlockPos != m_basicBlocks.end())
            return *m_curBlockPos;
        return nullptr;
    }
    bool hasInsertPoint() const {return m_curBlock != nullptr;}
    void setInsertPoint(std::shared_ptr<BasicBlock> bb) {m_curBlock = bb;}
    void clearInsertPoint() {m_curBlock = nullptr;}

    // 函数参数
    void addArgument(std::shared_ptr<Argument> arg) {m_arguments.push_back(arg);}
    PtrList<Argument> getArguments() {return m_arguments;}

    // 构造辅助函数
    void emitBlock(std::shared_ptr<BasicBlock> bb, bool isFinihed);
    void emitBranch(std::shared_ptr<BasicBlock> bb);

private:
    // 模块
    Module* m_module; 
    // 函数基本块
    std::shared_ptr<BasicBlock> m_curBlock;
    PtrList<BasicBlock> m_basicBlocks;
    // 函数参数
    PtrList<Argument> m_arguments; 
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