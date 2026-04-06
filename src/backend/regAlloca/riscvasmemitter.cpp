#include "riscvasmemitter.h"
#include "../target/risc_v/riscvframeinfo.h"
#include <sstream>

std::string RiscVAsmEmitter::operandToString(const MachineOperand& op)
{
    if (op.isVirtualReg()) {
        return "v" + std::to_string(op.getVReg());
    } else if (op.isPhysicalReg()) {
        return std::string(m_target->getRISCFrameInfo()->getRegName(op.getReg()));
    } else if (op.isImm()) {
        return std::to_string(op.getImm());
    } else if (op.isLabel()) {
        return op.getLabel();
    }
    return "?";
}

void RiscVAsmEmitter::emitLine(const std::string& line)
{
    for (int i = 0; i < m_indent; ++i) {
        m_output += " ";
    }
    m_output += line + "\n";
}

std::string RiscVAsmEmitter::instToString(MachineInst* inst)
{
    if (!inst) return "";

    std::ostringstream oss;
    auto opc = inst->getOpcode();
    const auto& operands = inst->getOperands();

    switch (opc) {
    case RiscvInstOpcode::NOP:
        return "nop";

    case RiscvInstOpcode::RET:
        return "ret";

    case RiscvInstOpcode::MV: {
        if (operands.size() >= 2) {
            oss << "mv " << operandToString(operands[0]) << ", " << operandToString(operands[1]);
        }
        break;
    }

    case RiscvInstOpcode::ADD: {
        if (operands.size() >= 3) {
            oss << "add " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::ADDI: {
        if (operands.size() >= 3) {
            oss << "addi " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::SUB: {
        if (operands.size() >= 3) {
            oss << "sub " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::MUL: {
        if (operands.size() >= 3) {
            oss << "mul " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::DIV: {
        if (operands.size() >= 3) {
            oss << "div " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::REM: {
        if (operands.size() >= 3) {
            oss << "rem " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::AND: {
        if (operands.size() >= 3) {
            oss << "and " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::ANDI: {
        if (operands.size() >= 3) {
            oss << "andi " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::OR: {
        if (operands.size() >= 3) {
            oss << "or " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::ORI: {
        if (operands.size() >= 3) {
            oss << "ori " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::XOR: {
        if (operands.size() >= 3) {
            oss << "xor " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::SLL: {
        if (operands.size() >= 3) {
            oss << "sll " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::SLLI: {
        if (operands.size() >= 3) {
            oss << "slli " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::SRL: {
        if (operands.size() >= 3) {
            oss << "srl " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::SRLI: {
        if (operands.size() >= 3) {
            oss << "srli " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::SRA: {
        if (operands.size() >= 3) {
            oss << "sra " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::SLT: {
        if (operands.size() >= 3) {
            oss << "slt " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::SLTI: {
        if (operands.size() >= 3) {
            oss << "slti " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::SLTU: {
        if (operands.size() >= 3) {
            oss << "sltu " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::SEQZ: {
        if (operands.size() >= 2) {
            oss << "seqz " << operandToString(operands[0]) << ", " << operandToString(operands[1]);
        }
        break;
    }

    case RiscvInstOpcode::SNEZ: {
        if (operands.size() >= 2) {
            oss << "snez " << operandToString(operands[0]) << ", " << operandToString(operands[1]);
        }
        break;
    }

    case RiscvInstOpcode::LW: {
        if (operands.size() >= 3) {
            oss << "lw " << operandToString(operands[0]) << ", "
                << operandToString(operands[2]) << "(" << operandToString(operands[1]) << ")";
        }
        break;
    }

    case RiscvInstOpcode::LB: {
        if (operands.size() >= 3) {
            oss << "lb " << operandToString(operands[0]) << ", "
                << operandToString(operands[2]) << "(" << operandToString(operands[1]) << ")";
        }
        break;
    }

    case RiscvInstOpcode::LBU: {
        if (operands.size() >= 3) {
            oss << "lbu " << operandToString(operands[0]) << ", "
                << operandToString(operands[2]) << "(" << operandToString(operands[1]) << ")";
        }
        break;
    }

    case RiscvInstOpcode::SW: {
        if (operands.size() >= 3) {
            oss << "sw " << operandToString(operands[0]) << ", "
                << operandToString(operands[2]) << "(" << operandToString(operands[1]) << ")";
        }
        break;
    }

    case RiscvInstOpcode::SB: {
        if (operands.size() >= 3) {
            oss << "sb " << operandToString(operands[0]) << ", "
                << operandToString(operands[2]) << "(" << operandToString(operands[1]) << ")";
        }
        break;
    }

    case RiscvInstOpcode::BEQZ: {
        if (operands.size() >= 2) {
            oss << "beqz " << operandToString(operands[0]) << ", " << operandToString(operands[1]);
        }
        break;
    }

    case RiscvInstOpcode::BNEZ: {
        if (operands.size() >= 2) {
            oss << "bnez " << operandToString(operands[0]) << ", " << operandToString(operands[1]);
        }
        break;
    }

    case RiscvInstOpcode::BEQ: {
        if (operands.size() >= 3) {
            oss << "beq " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::BNE: {
        if (operands.size() >= 3) {
            oss << "bne " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::BLT: {
        if (operands.size() >= 3) {
            oss << "blt " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::BGE: {
        if (operands.size() >= 3) {
            oss << "bge " << operandToString(operands[0]) << ", "
                << operandToString(operands[1]) << ", " << operandToString(operands[2]);
        }
        break;
    }

    case RiscvInstOpcode::JAL: {
        if (operands.size() >= 2) {
            oss << "jal " << operandToString(operands[0]) << ", " << operandToString(operands[1]);
        } else if (operands.size() >= 1) {
            oss << "jal " << operandToString(operands[0]);
        }
        break;
    }

    case RiscvInstOpcode::JALR: {
        if (operands.size() >= 1) {
            oss << "jalr " << operandToString(operands[0]);
        }
        break;
    }

    case RiscvInstOpcode::JR: {
        if (operands.size() >= 1) {
            oss << "jr " << operandToString(operands[0]);
        }
        break;
    }

    case RiscvInstOpcode::LI: {
        if (operands.size() >= 2) {
            oss << "li " << operandToString(operands[0]) << ", " << operandToString(operands[1]);
        }
        break;
    }

    case RiscvInstOpcode::LA: {
        if (operands.size() >= 2) {
            oss << "la " << operandToString(operands[0]) << ", " << operandToString(operands[1]);
        }
        break;
    }

    case RiscvInstOpcode::MOVE: {
        if (operands.size() >= 2) {
            oss << "mv " << operandToString(operands[0]) << ", " << operandToString(operands[1]);
        }
        break;
    }

    case RiscvInstOpcode::CALL: {
        if (operands.size() >= 1) {
            oss << "call " << operandToString(operands[0]);
        }
        break;
    }

    default:
        // 未知指令，输出注释
        oss << "# unknown opcode " << static_cast<int>(opc);
        break;
    }

    return oss.str();
}

void RiscVAsmEmitter::emitInst(MachineInst* inst)
{
    std::string line = instToString(inst);
    emitLine(line);
}

void RiscVAsmEmitter::emitBlock(MachineBlock* block)
{
    if (!block) return;

    // 发射基本块标签
    emitLine(block->getLabel() + ":");

    // 发射所有指令
    for (auto* inst : block->getInsts()) {
        emitInst(inst);
    }
}

void RiscVAsmEmitter::emitFunctionHeader()
{
    emitLine(".text");
    emitLine(".align 2");
    emitLine(".global " + m_function->getName());
    emitLine(".type " + m_function->getName() + ", @function");
    emitLine(m_function->getName() + ":");

    // 函数 prologue
    emitLine("    # prologue");
    emitLine("    addi sp, sp, -16");  // 分配栈帧 (简化)
    emitLine("    sw ra, 12(sp)");    // 保存返回地址
    emitLine("    sw s0, 8(sp)");     // 保存帧指针
    emitLine("    addi s0, sp, 16");  // 设置帧指针
}

void RiscVAsmEmitter::emitFunctionFooter()
{
    // 函数 epilogue
    emitLine("    # epilogue");
    emitLine("    lw ra, 12(sp)");    // 恢复返回地址
    emitLine("    lw s0, 8(sp)");     // 恢复帧指针
    emitLine("    addi sp, sp, 16");  // 释放栈帧
    emitLine("    ret");

    emitLine(".size " + m_function->getName() + ", .-" + m_function->getName());
}

std::string RiscVAsmEmitter::emit()
{
    m_output.clear();

    // 发射函数头
    emitFunctionHeader();

    // 发射所有基本块
    for (auto* block : m_function->getBlocks()) {
        emitBlock(block);
    }

    // 发射函数尾
    emitFunctionFooter();

    return m_output;
}
