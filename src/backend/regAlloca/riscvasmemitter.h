#pragma once

#include "../mir/machinefunction.h"
#include "../mir/machineblock.h"
#include "../mir/machineinst.h"
#include "../target/risc_v/riscvtargetmachine.h"
#include <string>

// RISC-V 汇编发射器
class RiscVAsmEmitter
{
public:
    RiscVAsmEmitter(RISCVTargetMachine* target, MachineFunction* func)
        : m_target(target), m_function(func), m_indent(0) {}

    // @brief: 发射整个函数的汇编代码
    std::string emit();

private:
    RISCVTargetMachine* m_target;
    MachineFunction* m_function;

    std::string m_output;
    int m_indent;

    // @brief: 发射指令
    void emitInst(MachineInst* inst);

    // @brief: 发射基本块
    void emitBlock(MachineBlock* block);

    // @brief: 发射函数头
    void emitFunctionHeader();

    // @brief: 发射函数尾
    void emitFunctionFooter();

    // @brief: 添加缩进
    void addIndent() { m_indent += 4; }
    void subIndent() { m_indent -= 4; }
    void emitLine(const std::string& line);

    // @brief: 获取操作数字符串
    std::string operandToString(const MachineOperand& op);

    // @brief: 指令到汇编字符串
    std::string instToString(MachineInst* inst);
};
