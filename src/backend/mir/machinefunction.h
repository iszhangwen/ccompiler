#pragma once

#include <list>

class MachineBlock;
class Function;

class MachineFunction
{
public:
    MachineFunction(Function* node): m_ssaFunction(node){}

    // @brief: 获取基本块
    std::list<MachineBlock*> getBlocks() {return m_blocks;}
    void addBlock(MachineBlock* block) {m_blocks.push_back(block);}

    // @brief:获取ssa ir函数
    Function* getSSAFunction() {return m_ssaFunction;}
    void setSSAFunction(Function* node) {m_ssaFunction = node;}

private:
    std::list<MachineBlock*> m_blocks;
    // ssa ir函数
    Function* m_ssaFunction;
    // 函数栈帧

};
