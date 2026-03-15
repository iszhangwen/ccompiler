#include "instruction.h"
#include "block.h"


BinaryInst::BinaryInst(OpCode opc, Value* v1, Value* v2, BasicBlock* bb)
: Instruction(opc, QualType(), "", 2, bb)
{
    setOperand(0, v1);
    setOperand(1, v2);
}

BinaryInst* BinaryInst::createAdd(Value* v1, Value* v2, BasicBlock* block)
{
    return Arena::make<BinaryInst>(OpCode::add, v1, v2, block);
}
BinaryInst* BinaryInst::createSub(Value* v1, Value* v2, BasicBlock* block)
{
    return Arena::make<BinaryInst>(OpCode::sub, v1, v2, block);
}
BinaryInst* BinaryInst::createMul(Value* v1, Value* v2, BasicBlock* block)
{
    return Arena::make<BinaryInst>(OpCode::mul, v1, v2, block);
}
BinaryInst* BinaryInst::createDiv(Value* v1, Value* v2, BasicBlock* block)
{
    return Arena::make<BinaryInst>(OpCode::div, v1, v2, block);
}
BinaryInst* BinaryInst::createFAdd(Value* v1, Value* v2, BasicBlock* block)
{
    return Arena::make<BinaryInst>(OpCode::float_add, v1, v2, block);
}
BinaryInst* BinaryInst::createFSub(Value* v1, Value* v2, BasicBlock* block)
{
    return Arena::make<BinaryInst>(OpCode::float_sub, v1, v2, block);
}
BinaryInst* BinaryInst::createFMul(Value* v1, Value* v2, BasicBlock* block)
{
    return Arena::make<BinaryInst>(OpCode::float_mul, v1, v2, block);
}
BinaryInst* BinaryInst::createFDiv(Value* v1, Value* v2, BasicBlock* block)
{
    return Arena::make<BinaryInst>(OpCode::float_div, v1, v2, block);
}


BranchInst::BranchInst(BasicBlock* parent, Value* cond, BasicBlock* ifThen, BasicBlock* ifElse)
: TerminatorInst(OpCode::branch, QualType(), "br", 3, parent) 
{
    if (cond) setOperand(0, cond);
    if (ifThen) setOperand(1, static_cast<Value*>(ifThen));
    if (ifElse) setOperand(2, static_cast<Value*>(ifElse));
}
// 无条件跳转构造：跳转目标只有一个块
BranchInst::BranchInst(BasicBlock* parent, BasicBlock* target)
: TerminatorInst(OpCode::branch, QualType(), "br", 1, parent) 
{
    setOperand(0, target);
}