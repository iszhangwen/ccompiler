#pragma once

#include "baseinst.h"

namespace ccompiler {

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

class CallInst : public Instruction
{
public:
    CallInst(QualType ty, Value* callee, BasicBlock* parent = nullptr)
    : Instruction(OpCode::Call, ty, parent) {
        resizeOperands(1);
        setOperand(0, callee);
    }

    Value* getCallee() { return getOperand(0); }
    int getNumArgs() const { return getOperandsNumber() - 1; }
    Value* getArg(int i) { return getOperand(i + 1); }

    void addArg(Value* arg) {
        int idx = getOperandsNumber();
        resizeOperands(idx + 1);
        setOperand(idx, arg);
    }
};
} // namespace ccompiler
