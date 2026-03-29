#pragma once

#include "baseinst.h"

class BinaryInst : public Instruction
{
public:
    explicit BinaryInst(OpCode opc, Value* v1, Value* v2, BasicBlock* bb);
    ~BinaryInst() = default;

    // @brief: 设置左表达式值
    Value* getLval();
    // @brief: 获取右表达式值
    void setLval(Value* val);

    // @brief: 设置右表达式值
    Value* getRval();
    // @brief: 获取右表达式值
    void setRval(Value* val);

    // @brief: 获取表达式的计算结果

    // @brief: 打印出IR
    void toStringPrint() override;
};

