#pragma once

#include "usedef.h"
#include "globalvalue.h"

#include <vector>
#include <stack>
#include <unordered_map>

class Module;
class BasicBlock;
class Argument;
class Instruction;

class Function : public GlobalValue
{
public:
    Function(QualType ty, const std::string& name, Module* parent);
    ~Function();

    // @brief: 获取函数返回类型
    QualType getRetType();

    // @brief: 获取函数形参列表
    void addArgument(Argument* arg) {m_arguments.push_back(arg);}
    // @brief: 设置函数形参列表
    std::list<Argument*> getArguments() {return m_arguments;}

    // @brief: 获取函数入口块
    BasicBlock* getEntryBlock() {return m_entrylock;}
    // @brief: 设置函数入口块
    void setEntryBlock(BasicBlock* bb) {m_entrylock = bb;}

    // 函数返回块
    void setReturnAddr(Instruction* ins) {m_returnAddr = ins;}
    Instruction* getReturnAddr() {return m_returnAddr;}
    // @brief: 获取函数返回块
    BasicBlock* getExitBlock() {return m_returnBlock;}
    // @brief: 设置函数返回块
    void setExitBlock(BasicBlock* bb) {m_returnBlock = bb;}

    // 基本块操作
    // 前驱子和后继子只有发生转移时才添加关系。
    void addBasicBlock(BasicBlock* bb) {m_basicBlocks.push_back(bb);}
    std::list<BasicBlock*> getBasicBlocks() {return m_basicBlocks;}
    BasicBlock* getInsertBlock() {return m_curBlock;}
    bool hasInsertPoint() const {return m_curBlock != nullptr;}
    void setInsertPoint(BasicBlock* bb) {m_curBlock = bb;}
    void clearInsertPoint() {m_curBlock = nullptr;}

    // @brief: 程序进入到bb基本块
    void emitBlock(BasicBlock* bb);
    // @brief: 在当前基本块插入br指令，无条件跳转到bb基本块
    void emitBranch(BasicBlock* bb);
    // @brief: 确保程序总是有插入点
    void ensureInsertPoint();

    // @brief: break和continue栈管理
    void pushBCStack(BasicBlock* B, BasicBlock* C);
    void popBCStack();
    std::pair<BasicBlock*, BasicBlock*> getBCBlock();

    // @brief: 局部变量声明地址获取
    Value* getLocalDeclAddr(NamedDecl* decl);
    // @brief: 记录局部变量声明地址
    void addLocalDeclAddr(NamedDecl* decl, Value* val);

    // @brief: 判断是否是内置函数
    bool isBuiltinFunction() const;
    // @brief: 判断是否是函数声明
    bool isDeclaration() const;

    // @brief: 创建基本块
    BasicBlock* createAndInsertBlock(const std::string& name);
    // @brief: 打印函数IR
    void toStringPrint();

private:
    // 是否是内置函数标志
    bool m_isBuiltin;
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


