#pragma once

#include <any>

class Module;
class Function;
class BasicBlock;
class GlobalVar;
class ConstantInt;
class ConstantFloat;
class ConstantString;
class ConstantChar;
class ConstantZero;
class AllocaInst;
class LoadInst;
class StoreInst;

// 通用访问接口
class IRVisitor 
{
public:
    virtual ~IRVisitor() = default;
    // 模块
    virtual std::any visit(Module*) = 0;
    virtual std::any visit(Function*) = 0;
    virtual std::any visit(BasicBlock*) = 0;
    // 全局变量
    virtual std::any visit(GlobalVar*) = 0;
    // 常量
    virtual std::any visit(ConstantInt*) = 0;
    virtual std::any visit(ConstantFloat*) = 0;
    virtual std::any visit(ConstantString*) = 0;
    virtual std::any visit(ConstantChar*) = 0;
    virtual std::any visit(ConstantZero*) = 0;
    // 指令
    virtual std::any visit(AllocaInst*) = 0;
    virtual std::any visit(LoadInst*) = 0;
    virtual std::any visit(StoreInst*) = 0;
};