#include "terminator.h"
#include "block.h"

#include <iostream>

BranchInst::BranchInst(BasicBlock* parent, Value* cond, BasicBlock* ifThen, BasicBlock* ifElse)
: TerminatorInst(OpCode::Branch, QualType(), parent)
{
    resizeOperands(3);
    if (cond) setOperand(0, cond);
    if (ifThen) setOperand(1, static_cast<Value*>(ifThen));
    if (ifElse) setOperand(2, static_cast<Value*>(ifElse));
}
// 无条件跳转构造：跳转目标只有一个块
BranchInst::BranchInst(BasicBlock* parent, BasicBlock* target)
: TerminatorInst(OpCode::Branch, QualType(), parent)
{
    resizeOperands(1);
    setOperand(0, target);
}

Value* BranchInst::getCond()
{
    if (isConditional())
        return getOperand(1);
    return nullptr;
}
void BranchInst::setCond(Value* val)
{
    if (isConditional())
        setOperand(0, val);
}

// @brief: 打印出IR
void BranchInst::toStringPrint() 
{
    std::cout << getName() << " ";
    if (isConditional()) {
        std::cout << getOperand(0)->getName() << " ";
        std::cout << getOperand(1)->getName() << " ";
        std::cout << getOperand(2)->getName() << "\n";
    } else {
        std::cout << getOperand(0)->getName() << "\n"; 
    }
}

ReturnInst::ReturnInst(QualType ty, BasicBlock* bb)
    : TerminatorInst(OpCode::Ret, ty, bb)
{
}

ReturnInst::ReturnInst(QualType ty, Value* val, BasicBlock* bb)
    : TerminatorInst(OpCode::Ret, ty, bb)
{
    resizeOperands(1);
    setOperand(0, val);
}

// @brief: 打印出IR
void ReturnInst::toStringPrint() 
{
    std::cout << getName() << " ";
    if (!isVoid()) {
        std::cout << getValue()->getName();
    }
    std::cout << "\n";
}