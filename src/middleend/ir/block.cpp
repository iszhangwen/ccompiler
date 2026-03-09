#include "block.h"
#include "instruction.h"

BasicBlock::BasicBlock(Module *m, Function *fun, const std::string &name)
: Value(QualType(), name), m_module(m), m_function(fun){
}

std::shared_ptr<BasicBlock> BasicBlock::create(Module* m, Function* func, const std::string &name) 
{
    return std::make_shared<BasicBlock>(m, func, name);
}

bool BasicBlock::isTerminator() const
{
    if (m_instructions.empty())
        return false;
    auto ins = m_instructions.back();
    if (ins == nullptr)
        return false;
    return ins->isTerminator();
}