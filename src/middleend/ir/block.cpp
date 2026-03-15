#include "block.h"
#include "instruction.h"

BasicBlock::BasicBlock(Module *m, Function *fun, const std::string &name)
: Value(QualType(), name), m_module(m), m_function(fun){
}

BasicBlock* BasicBlock::create(Module* m, Function* func, const std::string &name) 
{
    return Arena::make<BasicBlock>(m, func, name);
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