#include "usedef.h"
#include "arena.h"

void Value::replaceAllUseWith(Value* val)
{
    for (auto use: m_uses) {
        use.setValue(val);
    }
}

void User::resizeOperands(int size)
{
    m_operands.resize(size);
}

int User::getOperandsNumber() const
{
    return m_operands.size();
}

Value* User::getOperand(unsigned index) 
{
    if (index >= getOperandsNumber()) {
        return nullptr;
    }
    return m_operands[index];
}

void User::setOperand(unsigned index, Value* val) 
{
    if (index >= getOperandsNumber()) {
        return;
    }
    auto oldVal = m_operands[index];
    if (oldVal) {
        oldVal->removeUse(this);
    }
    m_operands[index] = val;  
    val->addUse(this);
}

void User::addOperand(Value* val) 
{
    m_operands.push_back(val); 
    val->addUse(this);
}
