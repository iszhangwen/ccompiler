#include "instselectpass.h"
#include "machinefunction.h"
#include "machineblock.h"
#include "function.h"
#include "block.h"
#include "arena.h"

// 简单的指令选择算法
bool InstSelectPass::runOnFunction(Function* node)
{
    // 1. 构建机器级表示
    m_function = Arena::make<MachineFunction>(node);
    // 2. 创建每个ssa函数的基本块
    for (auto block : node->getBasicBlocks()) {
        auto mblock = Arena::make<MachineBlock>(block);
        mblock->setLabel(block->getName());
        m_function->addBlock(mblock);
    }
    // 3. 递归下降
        // 2. 创建每个ssa函数的基本块
    for (auto mblock : m_function->getBlocks()) {
        selectBlock(mblock);
    }
    return true;
}

bool InstSelectPass::selectionBlock(MachineBlock* mblock)
{
    return true;
}

bool InstSelectPass::selectInstruction(MachineInst* minst)
{
    return true;
}