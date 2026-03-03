#include "block.h"
#include "instruction.h"


bool BasicBlock::isTerminator() const
{
    if (m_instructions.empty())
        return false;
    auto ins = m_instructions.back();
    if (ins == nullptr)
        return false;
    return ins->isTerminator();
}