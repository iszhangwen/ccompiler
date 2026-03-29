#pragma once

#include "baseinst.h"

// 从栈上分配一段内存，大小依据type决定
class AllocaInst : public Instruction
{
public:
    AllocaInst(QualType ty, BasicBlock* bb = nullptr);
    // @brief: 打印出IR
    void toStringPrint() override;
};
// 从地址获取值
class LoadInst : public Instruction
{
public:
    LoadInst(Value* ptr, BasicBlock* bb = nullptr);
    Value* getAddr() {return getOperand(0);}

    // @brief: 打印出IR
    void toStringPrint() override;
};
// 将值存入到地址
class StoreInst : public Instruction
{
public:
    StoreInst(Value* val, Value* ptr, BasicBlock* bb = nullptr);
    Value* getValue() {return getOperand(0);}
    Value* getAddr() {return getOperand(1);}

    // @brief: 打印出IR
    void toStringPrint() override;
};
