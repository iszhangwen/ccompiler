#pragma once

#include <variant>

class MachineBlock;
class Instruction;
class Value;

class MachineOperand
{
public:
    // @brief: 操作数类型
    enum MachineOperandKind {
        VREG, // 虚拟寄存器
        PREG, // 物理寄存器
        IMM,  // 立即数
    };

    MachineOperand(){}
    ~MachineOperand() = default;

    // @brief: 操作数类型判断函数
    bool isVirtualReg() const {return m_kind == VREG;}
    bool isPhysicalReg() const {return m_kind == PREG;}
    bool isImm() const {return m_kind == IMM;}

    // @brief: 属性操作
    MachineOperandKind getKind() {return m_kind};
    MachineType getType() {return m_type;}
    Value* getValue() {return m_value;}

private:
    // 数据值
    Value* m_value;
    // 操作数类型
    MachineOperandKind m_kind;
};

enum InstOpcode
{
    
}

class MachineInst
{
public:
    MachineInst(){}
    ~MachineInst() = default;

    // @brief: 所属的机器块
    MachineBlock* getParent() {return m_parent;}
    void setParent(MachineBlock* parent) {m_parent = parent;}

private:
    // 对应的ssa IR
    Instruction* m_ssaInst;
    // 所属的机器块
    MachineBlock* m_parent;
    // 指令对应的操作数
    std::vector<MachineOperand> m_operands;

};