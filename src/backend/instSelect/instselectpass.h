#pragma once

#include "pass.h"
#include "../mir/machinefunction.h"
#include "../mir/machineblock.h"
#include "../mir/machineinst.h"
#include "../mir/machinemodule.h"
#include "../target/risc_v/riscvtargetmachine.h"
#include "../target/risc_v/riscvframeinfo.h"
#include "inst/binaryinst.h"
#include "inst/meminst.h"
#include "inst/terminator.h"
#include "inst/otherinst.h"

class InstSelectPass : public FunctionPass
{
public:
    InstSelectPass() : m_targetMachine(nullptr), m_function(nullptr) {}
    InstSelectPass(RISCVTargetMachine* target) : m_targetMachine(target), m_function(nullptr) {}
    ~InstSelectPass() = default;

    // @brief: 获取机器模块
    MachineModule* getMachineModule() { return &m_machineModule; }

    // @brief: 设置目标机器
    void setTargetMachine(RISCVTargetMachine* target) { m_targetMachine = target; }

    // @brief: 实现指令选择
    bool runOnFunction(Function* node) override final;

    // @brief: 获取函数名
    std::string getName() const override { return m_passName; }

private:
    RISCVTargetMachine* m_targetMachine;
    MachineFunction* m_function;
    MachineModule m_machineModule;
    std::string m_passName = "Instruction Selection Pass";

    // @brief: 机器基本块指令选择
    void selectBlock(MachineBlock* mblock, BasicBlock* block);
    // @brief: 机器指令指令选择
    void selectInstruction(MachineBlock* mblock, Instruction* inst);

    // @brief: 为值分配虚拟寄存器
    int allocVReg(Value* val);

    // @brief: 获取值对应的虚拟寄存器
    int getValueVReg(Value* val);

    // 二元运算指令选择
    void selectBinaryInst(MachineBlock* mblock, BinaryInst* inst);

    // 内存访问指令选择
    void selectLoadInst(MachineBlock* mblock, LoadInst* inst);
    void selectStoreInst(MachineBlock* mblock, StoreInst* inst);
    void selectAllocaInst(MachineBlock* mblock, AllocaInst* inst);

    // Terminator指令选择
    void selectReturnInst(MachineBlock* mblock, ReturnInst* inst);
    void selectBranchInst(MachineBlock* mblock, BranchInst* inst);

    // PHI指令选择
    void selectPhiInst(MachineBlock* mblock, PhiInst* inst);

    // 创建机器指令
    MachineInst* createInst(RiscvInstOpcode opc) {
        auto* inst = new MachineInst(opc);
        return inst;
    }

    // 添加指令到基本块
    void addInst(MachineBlock* mblock, MachineInst* inst) {
        inst->setParent(mblock);
        mblock->addInst(inst);
    }
};
