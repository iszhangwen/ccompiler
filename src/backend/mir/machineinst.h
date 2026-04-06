#pragma once

#include <string>
#include <vector>
#include <memory>

#include "usedef.h"

class MachineBlock;
class Instruction;

// RISC-V 指令枚举
enum class RiscvInstOpcode {
    // 伪指令
    NOP, RET, MV, NOT, NEG, NEGW, ABS, BEQZ, BNEZ, BLEZ, BGEZ, BLTZ, BGTZ,

    // 算术运算
    ADD, ADDI, SUB, SUBI, MUL, DIV, REM,
    ADDW, SUBW, MULW, DIVW, REMW,
    ADDIW,

    // 位运算
    AND, ANDI, OR, ORI, XOR, XORI, SLL, SLLI, SRL, SRLI, SRA, SRAI,

    // 比较
    SLT, SLTI, SLTU, SLTIU, SEQZ, SNEZ,

    // 加载存储
    LB, LH, LW, LBU, LHU, SB, SH, SW,
    LIA,

    // 控制流
    JAL, JALR, JR, BEQ, BNE, BLT, BGE, BLTU, BGEU,

    // 函数调用
    CALL, TAIL,

    // 其他
    LI, LA, MOVE,
};

class MachineOperand
{
public:
    // @brief: 操作数类型
    enum MachineOperandKind {
        VREG, // 虚拟寄存器
        PREG, // 物理寄存器
        IMM,  // 立即数
        LABEL, // 标签
    };

    MachineOperand() : m_kind(VREG), m_value(nullptr), m_imm(0), m_isGlobal(false) {}
    ~MachineOperand() = default;

    // 创建虚拟寄存器操作数
    static MachineOperand createVReg(Value* val) {
        MachineOperand op;
        op.m_kind = VREG;
        op.m_value = val;
        return op;
    }

    // 创建物理寄存器操作数
    static MachineOperand createPReg(int reg) {
        MachineOperand op;
        op.m_kind = PREG;
        op.m_imm = reg;
        return op;
    }

    // 创建立即数操作数
    static MachineOperand createImm(int imm) {
        MachineOperand op;
        op.m_kind = IMM;
        op.m_imm = imm;
        return op;
    }

    // 创建标签操作数
    static MachineOperand createLabel(const std::string& label) {
        MachineOperand op;
        op.m_kind = LABEL;
        op.m_label = label;
        return op;
    }

    // 创建全局变量标签
    static MachineOperand createGlobal(const std::string& name) {
        MachineOperand op;
        op.m_kind = LABEL;
        op.m_label = name;
        op.m_isGlobal = true;
        return op;
    }

    // @brief: 操作数类型判断函数
    bool isVirtualReg() const { return m_kind == VREG; }
    bool isPhysicalReg() const { return m_kind == PREG; }
    bool isImm() const { return m_kind == IMM; }
    bool isLabel() const { return m_kind == LABEL; }
    bool isGlobal() const { return m_isGlobal; }

    // @brief: 属性操作
    MachineOperandKind getKind() const { return m_kind; }
    Value* getValue() const { return m_value; }
    int getImm() const { return m_imm; }
    const std::string& getLabel() const { return m_label; }
    int getReg() const { return m_imm; }

    // 获取虚拟寄存器编号
    int getVReg() const {
        if (m_value) return m_value->getVReg();
        return -1;
    }

private:
    // 数据值
    Value* m_value;
    // 操作数类型
    MachineOperandKind m_kind;
    // 立即数值或物理寄存器编号
    int m_imm{0};
    // 标签名
    std::string m_label;
    // 是否是全局变量
    bool m_isGlobal{false};
};

class MachineInst
{
public:
    MachineInst(RiscvInstOpcode opc) : m_opcode(opc), m_parent(nullptr), m_ssaInst(nullptr) {}
    ~MachineInst() = default;

    // 设置操作数
    void addOperand(MachineOperand op) { m_operands.push_back(op); }

    // 获取操作数
    const std::vector<MachineOperand>& getOperands() const { return m_operands; }
    std::vector<MachineOperand>& getOperands() { return m_operands; }
    MachineOperand getOperand(int idx) const { return m_operands.at(idx); }

    // @brief: 所属的机器块
    MachineBlock* getParent() { return m_parent; }
    void setParent(MachineBlock* parent) { m_parent = parent; }

    // @brief: 获取指令码
    RiscvInstOpcode getOpcode() const { return m_opcode; }

    // @brief: 对应的ssa IR
    Instruction* getSSAInst() const { return m_ssaInst; }
    void setSSAInst(Instruction* inst) { m_ssaInst = inst; }

    // @brief: 打印指令
    std::string toString() const;

private:
    RiscvInstOpcode m_opcode;
    MachineBlock* m_parent;
    Instruction* m_ssaInst;
    std::vector<MachineOperand> m_operands;
};