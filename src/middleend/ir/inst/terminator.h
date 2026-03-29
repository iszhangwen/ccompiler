#pragma once

#include "baseinst.h"

class TerminatorInst : public Instruction
{
public:
    TerminatorInst(OpCode opcode, QualType ty, BasicBlock* parent = nullptr)
    : Instruction(opcode, ty, parent) {}
    // 终结指令
    bool isTerminator() const override {return true;}
};

class ReturnInst : public TerminatorInst
{
public:
    ReturnInst(QualType ty, Value* val, BasicBlock* bb = nullptr);
    ReturnInst(QualType ty, BasicBlock* bb = nullptr);
    Value* getValue() {return getOperand(0);}

    // @brief: 判断是否是void函数
    bool isVoid() const {return getType()->isVoidType();}

    // @brief: 打印出IR
    void toStringPrint() override;
};

class BranchInst : public TerminatorInst
{
public:
    // @brief: 有条件跳转构造: 跳转目标是then块或者else块
    BranchInst(BasicBlock* parent, Value* cond, BasicBlock* ifThen, BasicBlock* ifElse);
    // @brief: 无条件跳转构造：跳转目标只有一个块
    BranchInst(BasicBlock* parent, BasicBlock* dest);

    // @brief: 判断是否是无条件跳转还是有条件跳转
    bool isUnconditional() const {return getOperandsNumber() == 1;}
    bool isConditional()   const {return getOperandsNumber() == 3;}

    // @brief:
    Value* getCond();
    // @brief:
    void setCond(Value* val);

    // @brief: 打印出IR
    void toStringPrint() override;
};
