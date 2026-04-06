#include "linearscan.h"
#include "../target/risc_v/riscvframeinfo.h"

// RISC-V 可用的整数寄存器 (排除 zero, sp, gp, tp)
const int LinearScanAllocPass::AVAIL_REGS[] = {
    RiscvReg::T0, RiscvReg::T1, RiscvReg::T2,
    RiscvReg::S0, RiscvReg::S1,
    RiscvReg::A0, RiscvReg::A1, RiscvReg::A2, RiscvReg::A3,
    RiscvReg::A4, RiscvReg::A5, RiscvReg::A6, RiscvReg::A7,
    RiscvReg::S2, RiscvReg::S3, RiscvReg::S4, RiscvReg::S5,
    RiscvReg::S6, RiscvReg::S7, RiscvReg::S8, RiscvReg::S9,
    RiscvReg::S10, RiscvReg::S11,
    RiscvReg::T3, RiscvReg::T4, RiscvReg::T5, RiscvReg::T6
};

void LinearScanAllocPass::initFreeRegs()
{
    m_freeRegs.clear();
    m_freeRegs.reserve(sizeof(AVAIL_REGS) / sizeof(AVAIL_REGS[0]));
    for (int reg : AVAIL_REGS) {
        m_freeRegs.push_back(reg);
    }
    m_allocatedRegs.clear();
}

int LinearScanAllocPass::allocateReg(int vreg)
{
    if (!m_freeRegs.empty()) {
        int preg = m_freeRegs.back();
        m_freeRegs.pop_back();
        m_allocatedRegs.insert(preg);
        return preg;
    }

    // 所有寄存器都被占用，需要溢出
    // 简单策略：溢出第一个已分配的寄存器
    if (!m_allocatedRegs.empty()) {
        int preg = *m_allocatedRegs.begin();
        // 找到这个物理寄存器对应的虚拟寄存器并溢出
        for (auto& pair : m_vreg2preg) {
            if (pair.second == preg) {
                // spill 这个虚拟寄存器
                spillReg(pair.first);
                m_vreg2preg.erase(pair.first);
                break;
            }
        }
        m_allocatedRegs.erase(preg);
        m_allocatedRegs.insert(preg);
        return preg;
    }

    // 失败，不应该到达这里
    return -1;
}

void LinearScanAllocPass::spillReg(int vreg)
{
    // 溢出处理：在栈上分配空间
    // 简单实现只记录需要溢出，实际的栈操作在代码生成时处理
}

int LinearScanAllocPass::getOrCreateMapping(int vreg)
{
    auto it = m_vreg2preg.find(vreg);
    if (it != m_vreg2preg.end()) {
        return it->second;
    }

    int preg = allocateReg(vreg);
    m_vreg2preg[vreg] = preg;
    return preg;
}

void LinearScanAllocPass::allocateForInst(MachineInst* inst)
{
    if (!inst) return;

    auto& operands = inst->getOperands();
    for (size_t i = 0; i < operands.size(); ++i) {
        if (operands[i].isVirtualReg()) {
            int vreg = operands[i].getVReg();
            if (vreg > 0) {
                int preg = getOrCreateMapping(vreg);
                // 更新操作数为物理寄存器
                operands[i] = MachineOperand::createPReg(preg);
            }
        }
    }
}

void LinearScanAllocPass::run(MachineFunction* func)
{
    m_function = func;
    m_vreg2preg.clear();

    // 初始化空闲寄存器
    initFreeRegs();

    // 遍历所有基本块的指令
    for (auto* block : func->getBlocks()) {
        if (!block) continue;

        for (auto* inst : block->getInsts()) {
            allocateForInst(inst);
        }
    }

    // 将映射设置到函数中
    for (auto& pair : m_vreg2preg) {
        func->addVRegMapping(pair.first, pair.second);
    }
}
