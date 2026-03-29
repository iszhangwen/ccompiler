#pragma once

#include "baseinst.h"

class PhiInst : public Instruction
{
public:
    PhiInst(QualType ty, BasicBlock* parent)
    : Instruction(OpCode::Phi, ty, parent){}
    Value* getValue() {return m_value;}
    void setValue(Value* val) {
        val->addUse(this);
        m_value = val;
    }

    void addIncoming(Value* val, BasicBlock* bb);
    std::vector<std::pair<Value*, BasicBlock*>> getIncoming();

private:
    Value* m_value;
};
