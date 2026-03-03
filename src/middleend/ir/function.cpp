#include "function.h"
#include "block.h"

void Function::emitBlock(std::shared_ptr<BasicBlock> bb, bool isFinihed)
{
    emitBranch()
}

void Function::emitBranch(std::shared_ptr<BasicBlock> bb)
{
    auto curBlock = getInsertBlock();
    if (!curBlock || curBlock->isTerminator()) {
        // 如果当前基本块为空或者已经没有了插入点 
        // 则什么也不做  
    } else {
        // 否则再当前块的末尾插入跳转指令
        // 跳转指令的跳转目标是新的基本块
    }
    clearInsertPoint();
}