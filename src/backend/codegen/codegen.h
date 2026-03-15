#pragma once

#include <string>
#include <vector>
#include "irvisitor.h"

// 通用访问接口
class CodegenAssembler : public IRVisitor 
{
public:
    virtual ~CodegenAssembler() = default;
    // 模块
    virtual std::any visit(Module*) override;
    virtual std::any visit(Function*) override;
    virtual std::any visit(BasicBlock*) override;
    // 全局变量
    virtual std::any visit(GlobalVar*) override;
    // 常量
    virtual std::any visit(ConstantInt*) override;
    virtual std::any visit(ConstantFloat*) override;
    virtual std::any visit(ConstantString*) override;
    virtual std::any visit(ConstantChar*) override;
    virtual std::any visit(ConstantZero*) override;
    // 指令
    virtual std::any visit(AllocaInst*) override;
    virtual std::any visit(LoadInst*) override;
    virtual std::any visit(StoreInst*) override;
};