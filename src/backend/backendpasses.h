#pragma once

#include "mir/machinemodule.h"
#include "target/risc_v/riscvtargetmachine.h"
#include "instSelect/instselectpass.h"
#include "regAlloca/linearscan.h"
#include <string>

class Module;

// 后端Pass管理器
class BackendPassManager
{
public:
    BackendPassManager() : m_targetMachine(nullptr), m_asmOutput() {}

    // @brief: 设置目标机器
    void setTargetMachine(RISCVTargetMachine* target) { m_targetMachine = target; }

    // @brief: 运行所有后端pass
    void run(Module* ssaModule);

    // @brief: 获取汇编输出
    const std::string& getAsmOutput() const { return m_asmOutput; }

    // @brief: 获取机器模块
    MachineModule* getMachineModule() { return m_instSelect.getMachineModule(); }

private:
    RISCVTargetMachine* m_targetMachine;
    InstSelectPass m_instSelect;
    std::string m_asmOutput;

    // @brief: 运行寄存器分配
    void runRegAlloc();

    // @brief: 发射汇编
    void emitAsm();
};
