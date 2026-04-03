#pragma once
#include "pass.h"

class TargetMachine;
class MachineFunction;

class InstSelectPass : public FunctionPass
{
public:
    InstSelectPass(TargetMachine* target):m_targetMachine(target){}
    ~InstSelectPass() = default;

    // @brief: 实现指令选择
    bool runOnFunction(Function* node) override final; 

private:
    TargetMachine* m_targetMachine;
    MachineFunction* m_function;

    // @brief: 机器基本块指令选择
    void selectBlock(MachineBlock*);
    // @brief: 机器指令指令选择
    void selectInstruction(MachineInst*);
};
