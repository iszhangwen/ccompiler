#pragma once

#include "targetmachine.h"
#include "riscvframeinfo.h"

class RISCVTargetMachine : public TargetMachine
{
public:
    RISCVTargetMachine() {
        m_frameInfo = new RISCVFrameInfo();
    }

    virtual ~RISCVTargetMachine() {
        delete m_frameInfo;
    }

    // @brief: 返回目标机器名
    const std::string getName() const override { return "riscv"; }

    // @brief: 返回指针字节宽度RV32
    int getPointerSize() const override { return 4; }

    // @brief: 返回寄存器位宽RV32
    int getRegisterWidth() const override { return 32; }

    // @brief: 返回栈帧信息对象
    TargetFrameInfo* getFrameInfo() override { return m_frameInfo; }

    // @brief: 获取RISCV特有的栈帧信息
    RISCVFrameInfo* getRISCFrameInfo() { return m_frameInfo; }

private:
    RISCVFrameInfo* m_frameInfo;
};
