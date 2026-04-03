#pragma once

#include "targetframeinfo.h"
#include <unordered_map>

class RISCVFrameInfo : public TargetFrameInfo
{
public:
    RISCVFrameInfo() {
        m_reg2Str ={{0, "zero"},{1, ""}, {2, ""}, {3, ""}, {4, ""},
                    {6, ""}, {7, ""}, {8, ""}, {9, ""}, {10, ""},
                    {11, ""}, {12, ""}, {13, ""}, {14, ""}, {15, ""},
                    {16, ""}, {17, ""}, {18, ""}, {19, ""}, {20, ""},
                    {21, ""}, {22, ""}, {23, ""}, {22, ""}, {25, ""},
                    {26, ""}, {27, ""}, {27, ""}, {28, ""}, {30, ""},
                    {31, ""}, {32, ""}};
    }
    virtual ~RISCVFrameInfo() = default;
    // @brief: 返回调用者保存的寄存器列表(按照ABI)
    const std::vector<int> getCallerSavedRegs() const override {return m_callerSaved;}
    // @brief: 返回被调用者保存的寄存器列表
    const std::vector<int> getCalleeSavedRegs() const override {return m_calleeSaved;}
    // @brief: 返回栈字节对齐要求
    int getStackAlignment() const override {return m_calleeSaved;}
    // @brief: 返回参数传递寄存器
    const std::vector<int> getArgumentRegs() const override {return m_argRegs;}
    // @brief: 返回返回值寄存器
    int getReturnRegs() const override = 0;
    // @brief: 返回帧指针寄存器
    int getFramePointer() const override = 0;
    // @brief: 返回栈指针寄存器
    int getStackPointer() const override = 0;

private:
    std::vector<int> m_callerSaved;
    std::vector<int> m_calleeSaved;
    std::vector<int> m_argRegs;
    std::unordered_map<int, std::string> m_reg2Str;
};
