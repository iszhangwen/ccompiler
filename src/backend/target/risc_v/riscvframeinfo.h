#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "../targetframeinfo.h"

// RISC-V ABI寄存器编号
namespace RiscvReg {
    enum {
        ZERO = 0, RA = 1, SP = 2, GP = 3, TP = 4,
        T0 = 5, T1 = 6, T2 = 7,
        S0 = 8, S1 = 9,
        A0 = 10, A1 = 11, A2 = 12, A3 = 13, A4 = 14, A5 = 15,
        A6 = 16, A7 = 17,
        S2 = 18, S3 = 19, S4 = 20, S5 = 21, S6 = 22, S7 = 23,
        S8 = 24, S9 = 25, S10 = 26, S11 = 27,
        T3 = 28, T4 = 29, T5 = 30, T6 = 31
    };
}

class RISCVFrameInfo : public TargetFrameInfo
{
public:
    RISCVFrameInfo() {
        // 调用者保存寄存器 (caller-saved / temporary)
        m_callerSaved = {
            RiscvReg::RA,   // 返回地址
            RiscvReg::T0, RiscvReg::T1, RiscvReg::T2,  // temporaries
            RiscvReg::T3, RiscvReg::T4, RiscvReg::T5, RiscvReg::T6,
            RiscvReg::A0, RiscvReg::A1, RiscvReg::A2, RiscvReg::A3,  // arguments / return
            RiscvReg::A4, RiscvReg::A5, RiscvReg::A6, RiscvReg::A7
        };

        // 被调用者保存寄存器 (callee-saved / preserved)
        m_calleeSaved = {
            RiscvReg::S0, RiscvReg::S1,  // saved registers
            RiscvReg::S2, RiscvReg::S3, RiscvReg::S4, RiscvReg::S5,
            RiscvReg::S6, RiscvReg::S7, RiscvReg::S8, RiscvReg::S9,
            RiscvReg::S10, RiscvReg::S11
        };

        // 参数传递寄存器 (前8个整数参数)
        m_argRegs = {
            RiscvReg::A0, RiscvReg::A1, RiscvReg::A2, RiscvReg::A3,
            RiscvReg::A4, RiscvReg::A5, RiscvReg::A6, RiscvReg::A7
        };

        // 寄存器名称映射
        m_reg2Str = {
            {0, "zero"}, {1, "ra"}, {2, "sp"}, {3, "gp"}, {4, "tp"},
            {5, "t0"}, {6, "t1"}, {7, "t2"},
            {8, "s0"}, {9, "s1"},
            {10, "a0"}, {11, "a1"}, {12, "a2"}, {13, "a3"},
            {14, "a4"}, {15, "a5"}, {16, "a6"}, {17, "a7"},
            {18, "s2"}, {19, "s3"}, {20, "s4"}, {21, "s5"},
            {22, "s6"}, {23, "s7"}, {24, "s8"}, {25, "s9"},
            {26, "s10"}, {27, "s11"},
            {28, "t3"}, {29, "t4"}, {30, "t5"}, {31, "t6"}
        };
    }

    virtual ~RISCVFrameInfo() = default;

    // @brief: 返回调用者保存的寄存器列表(按照ABI)
    const std::vector<int> getCallerSavedRegs() const override { return m_callerSaved; }

    // @brief: 返回被调用者保存的寄存器列表
    const std::vector<int> getCalleeSavedRegs() const override { return m_calleeSaved; }

    // @brief: 返回栈字节对齐要求 (RISC-V ABI要求16字节对齐)
    int getStackAlignment() const override { return 16; }

    // @brief: 返回参数传递寄存器
    const std::vector<int> getArgumentRegs() const override { return m_argRegs; }

    // @brief: 返回返回值寄存器
    int getReturnRegs() const override { return RiscvReg::A0; }

    // @brief: 返回帧指针寄存器 (s0/fp)
    int getFramePointer() const override { return RiscvReg::S0; }

    // @brief: 返回栈指针寄存器
    int getStackPointer() const override { return RiscvReg::SP; }

    // @brief: 获取寄存器名称
    const char* getRegName(int reg) const {
        auto it = m_reg2Str.find(reg);
        if (it != m_reg2Str.end()) {
            return it->second.c_str();
        }
        return "unknown";
    }

private:
    std::vector<int> m_callerSaved;
    std::vector<int> m_calleeSaved;
    std::vector<int> m_argRegs;
    std::unordered_map<int, std::string> m_reg2Str;
};
