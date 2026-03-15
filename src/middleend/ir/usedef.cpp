#include "usedef.h"


Value* User::getOperand(unsigned index) 
{
    assert(index < m_operands.size() && "getOperand out of range");
    return m_operands[index];
}

void User::setOperand(unsigned index, Value* val) 
{
    assert(index < m_operands.size() && "setOperand() out of range!");
    auto beforeVal = m_operands[index];
    if (beforeVal) {
        beforeVal->removeUse(this);
    }
    m_operands[index] = val;  
    val->addUse(this);
}

void User::addOperand(Value* val) 
{
    m_operandNum++;
    m_operands.push_back(val); 
    val->addUse(this);
}
