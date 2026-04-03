#pragma once

#include <list>

class MachineFucntion;
class MachineInst;
class BasicBlock;

class MachineBlock
{
public:
    MachineBlock(BasicBlock* block): m_ssaBlock(block){}
    ~MachineBlock() = default;

    // @brief: 获取前驱子与后继子
    std::list<MachineBlock*> getPredecessors() {return m_predecessors;}
    void addPredecess(MachineBlock* block) {m_predecessors.push_back(block);}
    std::list<MachineBlock*> getSuccessors() {return m_successors;}
    void addSuccessor(MachineBlock* block) {m_successors.push_back(block);}

    // @brief: 指令操作
    std::list<MachineInst*> getInst() {return m_instructions;}
    void addInst(MachineInst* inst) {m_instructions.push_back(inst);}

    // @brief: 所属函数操作
    MachineFucntion* getParent() {return m_parent;}
    void setParent(MachineFucntion* parent) {m_parent = parent;}

    // @brief: 基本块的label;
    void setLabel(const std::string& label) {m_label = label;}
    const std::string& getLabel() const {return m_label;}

private:
    // 基本块Label
    std::string m_label;
    // 对应的SSA IR基本块
    BasicBlock* m_ssaBlock;
    // 所属的machine函数
    MachineFucntion* m_parent;
    // 指令集合: alloca指令和其他指令
    std::list<MachineInst*> m_instructions;
    // 前驱子与后继子
    std::list<MachineBlock*> m_predecessors;
    std::list<MachineBlock*> m_successors;
};