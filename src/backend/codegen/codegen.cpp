#include "codegen.h"

std::any CodegenAssembler::visit(Module*) 
{
    return std::any();
}

std::any CodegenAssembler::visit(Function*) 
{
    return std::any();
}

std::any CodegenAssembler::visit(BasicBlock*) 
{
    return std::any();
}

// 全局变量
std::any CodegenAssembler::visit(GlobalVar*) 
{
    return std::any();
}

// 常量
std::any CodegenAssembler::visit(ConstantInt*) 
{
    return std::any();
}

std::any CodegenAssembler::visit(ConstantFloat*) 
{
    return std::any();
}

std::any CodegenAssembler::visit(ConstantString*) 
{
    return std::any();
}

std::any CodegenAssembler::visit(ConstantChar*) 
{
    return std::any();
}

std::any CodegenAssembler::visit(ConstantZero*) 
{
    return std::any();
}

// 指令
std::any CodegenAssembler::visit(AllocaInst*) 
{
    return std::any();
}

std::any CodegenAssembler::visit(LoadInst*) 
{
    return std::any();
}

std::any CodegenAssembler::visit(StoreInst*) 
{
    return std::any();
}