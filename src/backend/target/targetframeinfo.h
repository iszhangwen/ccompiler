#pragma once

#include <vector>

// @brief: 抽象的栈帧信息
class TargetFrameInfo
{
public:
    virtual ~TargetFrameInfo() = default;
    // @brief: 返回调用者保存的寄存器列表(按照ABI)
    virtual const std::vector<int> getCallerSavedRegs() const = 0;
    // @brief: 返回被调用者保存的寄存器列表
    virtual const std::vector<int> getCalleeSavedRegs() const = 0;
    // @brief: 返回栈字节对齐要求
    virtual int getStackAlignment() const = 0;
    // @brief: 返回参数传递寄存器
    virtual const std::vector<int> getArgumentRegs() const = 0;
    // @brief: 返回返回值寄存器
    virtual int getReturnRegs() const = 0;
    // @brief: 返回帧指针寄存器
    virtual int getFramePointer() const = 0;
    // @brief: 返回栈指针寄存器
    virtual int getStackPointer() const = 0;
};
