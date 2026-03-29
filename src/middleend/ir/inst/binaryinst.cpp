#include "binaryinst.h"
#include "block.h"

#include <iostream>

BinaryInst::BinaryInst(OpCode opc, Value* v1, Value* v2, BasicBlock* bb)
: Instruction(opc, QualType(), bb)
{
    resizeOperands(2);
    setOperand(0, v1);
    setOperand(1, v2);
}

// @brief: 设置左表达式值
Value* BinaryInst::getLval()
{
    return getOperand(0);
}
// @brief: 获取右表达式值
void BinaryInst::setLval(Value* val)
{
    setOperand(0, val);
}

// @brief: 设置右表达式值
Value* BinaryInst::getRval()
{
    return getOperand(0);
}
// @brief: 获取右表达式值
void BinaryInst::setRval(Value* val)
{
    setOperand(1, val);
}

// @brief: 打印出IR
void BinaryInst::toStringPrint() 
{
    std::cout << getName() << " ";
    std::cout << getOperand(0)->getName() << " ";
    std::cout << getOperand(1)->getName() << " ";
    std::cout << "\n";
}


