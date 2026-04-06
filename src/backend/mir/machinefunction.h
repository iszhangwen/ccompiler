#pragma once

#include <list>
#include <string>
#include <vector>
#include <unordered_map>

#include "machineblock.h"

class Function;
class RISCVFrameInfo;

class MachineFunction
{
public:
    MachineFunction(Function* node): m_ssaFunction(node), m_frameInfo(nullptr) {}

    // @brief: 获取基本块
    std::list<MachineBlock*> getBlocks() { return m_blocks; }
    void addBlock(MachineBlock* block) {
        block->setParent(this);
        m_blocks.push_back(block);
    }

    // @brief: 获取ssa ir函数
    Function* getSSAFunction() { return m_ssaFunction; }
    void setSSAFunction(Function* node) { m_ssaFunction = node; }

    // @brief: 获取函数名
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    // @brief: 栈帧信息
    void setFrameInfo(RISCVFrameInfo* info) { m_frameInfo = info; }
    RISCVFrameInfo* getFrameInfo() { return m_frameInfo; }

    // @brief: 添加虚拟寄存器到物理寄存器的映射
    void addVRegMapping(int vreg, int preg) { m_vreg2preg[vreg] = preg; }
    int getVRegMapping(int vreg) {
        auto it = m_vreg2preg.find(vreg);
        return (it != m_vreg2preg.end()) ? it->second : -1;
    }

    // @brief: 获取所有虚拟寄存器
    const std::vector<int>& getVirtualRegisters() const { return m_virtualRegs; }
    void addVirtualRegister(int vreg) { m_virtualRegs.push_back(vreg); }

    // @brief: 设置需要保存的寄存器
    void setCalleeSaved(const std::vector<int>& regs) { m_calleeSaved = regs; }
    const std::vector<int>& getCalleeSaved() const { return m_calleeSaved; }

private:
    std::string m_name;
    std::list<MachineBlock*> m_blocks;
    Function* m_ssaFunction;
    RISCVFrameInfo* m_frameInfo;
    std::unordered_map<int, int> m_vreg2preg;
    std::vector<int> m_virtualRegs;
    std::vector<int> m_calleeSaved;
};
