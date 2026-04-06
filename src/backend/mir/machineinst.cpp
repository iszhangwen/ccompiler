#include "machineinst.h"
#include "machineblock.h"
#include "../../middleend/ir/inst/baseinst.h"

// RISC-V 寄存器名称
static const char* const RiscvRegNames[] = {
    "zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
    "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
    "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
    "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

static const char* getRegName(int reg) {
    if (reg >= 0 && reg < 32) {
        return RiscvRegNames[reg];
    }
    return "unknown";
}

static std::string opcodeToString(RiscvInstOpcode opc) {
    switch (opc) {
    case RiscvInstOpcode::NOP: return "nop";
    case RiscvInstOpcode::RET: return "ret";
    case RiscvInstOpcode::MV: return "mv";
    case RiscvInstOpcode::NOT: return "not";
    case RiscvInstOpcode::NEG: return "neg";
    case RiscvInstOpcode::NEGW: return "negw";
    case RiscvInstOpcode::ABS: return "abs";
    case RiscvInstOpcode::BEQZ: return "beqz";
    case RiscvInstOpcode::BNEZ: return "bnez";
    case RiscvInstOpcode::BLEZ: return "blez";
    case RiscvInstOpcode::BGEZ: return "bgez";
    case RiscvInstOpcode::BLTZ: return "bltz";
    case RiscvInstOpcode::BGTZ: return "bgtz";
    case RiscvInstOpcode::ADD: return "add";
    case RiscvInstOpcode::ADDI: return "addi";
    case RiscvInstOpcode::SUB: return "sub";
    case RiscvInstOpcode::SUBI: return "subi";
    case RiscvInstOpcode::MUL: return "mul";
    case RiscvInstOpcode::DIV: return "div";
    case RiscvInstOpcode::REM: return "rem";
    case RiscvInstOpcode::ADDW: return "addw";
    case RiscvInstOpcode::SUBW: return "subw";
    case RiscvInstOpcode::MULW: return "mulw";
    case RiscvInstOpcode::DIVW: return "divw";
    case RiscvInstOpcode::REMW: return "remw";
    case RiscvInstOpcode::ADDIW: return "addiw";
    case RiscvInstOpcode::AND: return "and";
    case RiscvInstOpcode::ANDI: return "andi";
    case RiscvInstOpcode::OR: return "or";
    case RiscvInstOpcode::ORI: return "ori";
    case RiscvInstOpcode::XOR: return "xor";
    case RiscvInstOpcode::XORI: return "xori";
    case RiscvInstOpcode::SLL: return "sll";
    case RiscvInstOpcode::SLLI: return "slli";
    case RiscvInstOpcode::SRL: return "srl";
    case RiscvInstOpcode::SRLI: return "srli";
    case RiscvInstOpcode::SRA: return "sra";
    case RiscvInstOpcode::SRAI: return "srai";
    case RiscvInstOpcode::SLT: return "slt";
    case RiscvInstOpcode::SLTI: return "slti";
    case RiscvInstOpcode::SLTU: return "sltu";
    case RiscvInstOpcode::SLTIU: return "sltiu";
    case RiscvInstOpcode::SEQZ: return "seqz";
    case RiscvInstOpcode::SNEZ: return "snez";
    case RiscvInstOpcode::LB: return "lb";
    case RiscvInstOpcode::LH: return "lh";
    case RiscvInstOpcode::LW: return "lw";
    case RiscvInstOpcode::LBU: return "lbu";
    case RiscvInstOpcode::LHU: return "lhu";
    case RiscvInstOpcode::SB: return "sb";
    case RiscvInstOpcode::SH: return "sh";
    case RiscvInstOpcode::SW: return "sw";
    case RiscvInstOpcode::LIA: return "lia";
    case RiscvInstOpcode::JAL: return "jal";
    case RiscvInstOpcode::JALR: return "jalr";
    case RiscvInstOpcode::JR: return "jr";
    case RiscvInstOpcode::BEQ: return "beq";
    case RiscvInstOpcode::BNE: return "bne";
    case RiscvInstOpcode::BLT: return "blt";
    case RiscvInstOpcode::BGE: return "bge";
    case RiscvInstOpcode::BLTU: return "bltu";
    case RiscvInstOpcode::BGEU: return "bgeu";
    case RiscvInstOpcode::CALL: return "call";
    case RiscvInstOpcode::TAIL: return "tail";
    case RiscvInstOpcode::LI: return "li";
    case RiscvInstOpcode::LA: return "la";
    case RiscvInstOpcode::MOVE: return "move";
    default: return "unknown";
    }
}

std::string MachineInst::toString() const {
    std::string result = opcodeToString(m_opcode);

    if (m_operands.empty()) {
        return result;
    }

    result += " ";
    bool first = true;
    for (const auto& op : m_operands) {
        if (!first) result += ", ";
        first = false;

        if (op.isVirtualReg()) {
            result += "v" + std::to_string(op.getVReg());
        } else if (op.isPhysicalReg()) {
            result += getRegName(op.getReg());
        } else if (op.isImm()) {
            result += std::to_string(op.getImm());
        } else if (op.isLabel()) {
            result += op.getLabel();
        }
    }

    return result;
}
