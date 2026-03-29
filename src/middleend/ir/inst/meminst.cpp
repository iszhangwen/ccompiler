#include "meminst.h"

#include <iostream>

AllocaInst::AllocaInst(QualType ty, BasicBlock* bb)
: Instruction(OpCode::Alloca, ty, bb) {
    setName("Alloca_" + std::to_string(uint64_t(this)));
}

// @brief: 打印出IR
void AllocaInst::toStringPrint() 
{
    std::cout << getName() << "\n";
}

LoadInst::LoadInst(Value* ptr, BasicBlock* bb)
: Instruction(OpCode::Load, QualType(), bb) {
    resizeOperands(1);
    setOperand(0, ptr);
}

// @brief: 打印出IR
void LoadInst::toStringPrint() 
{
    std::cout << getName() << " ";
    std::cout << getAddr()->getName() << "\n";
}

StoreInst::StoreInst(Value* val, Value* ptr, BasicBlock* bb)
: Instruction(OpCode::Store, QualType(), bb) {
    resizeOperands(2);
    setOperand(0, val);
    setOperand(1, ptr);
}

// @brief: 打印出IR
void StoreInst::toStringPrint() 
{
    std::cout << getName() << " ";
    std::cout << getValue()->getName() << " ";
    std::cout << getAddr()->getName() << "\n";
}