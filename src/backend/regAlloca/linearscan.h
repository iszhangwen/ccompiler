#pragma once

#include "../mir/machinefunction.h"
#include "../mir/machineblock.h"
#include "../mir/machineinst.h"
#include "../target/risc_v/riscvtargetmachine.h"
#include <vector>
#include <map>
#include <set>

// 线性扫描寄存器分配
class LinearScanAllocPass
{
public:
    LinearScanAllocPass(RISCVTargetMachine* target) : m_targetMachine(target) {}

    // @brief: 运行寄存器分配
    void run(MachineFunction* func);

    // @brief: 获取虚拟寄存器到物理寄存器的映射
    const std::map<int, int>& getVRegMap() const { return m_vreg2preg; }

private:
    RISCVTargetMachine* m_targetMachine;
    MachineFunction* m_function;
    std::map<int, int> m_vreg2preg;  // 虚拟寄存器到物理寄存器的映射
    std::vector<int> m_freeRegs;      // 空闲物理寄存器列表
    std::set<int> m_allocatedRegs;    // 已分配的物理寄存器

    // RISC-V 可用寄存器 (不包含zero和一些特殊寄存器)
    static const int AVAIL_REGS[];

    // @brief: 初始化空闲寄存器列表
    void initFreeRegs();

    // @brief: 分配物理寄存器给虚拟寄存器
    int allocateReg(int vreg);

    // @brief: 获取或创建虚拟寄存器映射
    int getOrCreateMapping(int vreg);

    // @brief: 溢出处理 - 将寄存器 spill 到栈
    void spillReg(int vreg);

    // @brief: 为指令操作数分配寄存器
    void allocateForInst(MachineInst* inst);
};
