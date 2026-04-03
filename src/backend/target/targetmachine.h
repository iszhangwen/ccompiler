#pragma once

#include <vector>
#include "targetframeinfo.h"

// @brief: 抽象目标机器
class TargetMachine
{
public:
    virtual ~TargetMachine() = default;
    // @brief: 返回目标机器名
    virtual const std::string getName() const = 0;
    // @brief: 返回指针字节宽度
    virtual int getPointerSize() const = 0;
    // @brief: 返回寄存器位宽
    virtual int getRegisterWidth() const = 0;
    // @brief: 返回栈帧信息对象
    virtual class TargetFrameInfo* getFrameInfo() = 0;
};

