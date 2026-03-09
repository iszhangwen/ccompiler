#include "instruction.h"
#include "block.h"


BinaryInst::BinaryInst(OpCode opc, std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* bb)
: Instruction(opc, QualType(), "", 2, bb)
{
    setOperand(0, v1);
    setOperand(1, v2);
}

std::shared_ptr<BinaryInst> BinaryInst::createAdd(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block)
{
    return std::make_shared<BinaryInst>(OpCode::add, v1, v2, block);
}
std::shared_ptr<BinaryInst> BinaryInst::createSub(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block)
{
    return std::make_shared<BinaryInst>(OpCode::sub, v1, v2, block);
}
std::shared_ptr<BinaryInst> BinaryInst::createMul(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block)
{
    return std::make_shared<BinaryInst>(OpCode::mul, v1, v2, block);
}
std::shared_ptr<BinaryInst> BinaryInst::createDiv(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block)
{
    return std::make_shared<BinaryInst>(OpCode::div, v1, v2, block);
}
std::shared_ptr<BinaryInst> BinaryInst::createFAdd(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block)
{
    return std::make_shared<BinaryInst>(OpCode::float_add, v1, v2, block);
}
std::shared_ptr<BinaryInst> BinaryInst::createFSub(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block)
{
    return std::make_shared<BinaryInst>(OpCode::float_sub, v1, v2, block);
}
std::shared_ptr<BinaryInst> BinaryInst::createFMul(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block)
{
    return std::make_shared<BinaryInst>(OpCode::float_mul, v1, v2, block);
}
std::shared_ptr<BinaryInst> BinaryInst::createFDiv(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block)
{
    return std::make_shared<BinaryInst>(OpCode::float_div, v1, v2, block);
}


BranchInst::BranchInst(BasicBlock* parent, std::shared_ptr<Value> cond, BasicBlock* ifThen, BasicBlock* ifElse)
: TerminatorInst(OpCode::branch, QualType(), "br", 3, parent) 
{
    if (cond) setOperand(0, cond);
    if (ifThen) setOperand(1, static_cast<std::shared_ptr<Value>>(ifThen));
    if (ifElse) setOperand(2, static_cast<std::shared_ptr<Value>>(ifElse));
}
// 无条件跳转构造：跳转目标只有一个块
BranchInst::BranchInst(BasicBlock* parent, BasicBlock* target)
: TerminatorInst(OpCode::branch, QualType(), "br", 1, parent) 
{
    setOperand(0, std::shared_ptr<Value>(target));
}