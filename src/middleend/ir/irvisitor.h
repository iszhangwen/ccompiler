#pragma once

#include "instruction.h"

class SSAIRVisitor
{
public:
    virtual void visit(AllocaInst*) = 0;
    virtual void visit(LoadInst*) = 0;
    virtual void visit(StoreInst*) = 0;
    virtual void visit(ReturnInst*) = 0;
    virtual void visit(BranchInst*) = 0;
    virtual void visit(PhiInst*) = 0;
    virtual void visit(BinaryInst*) = 0;
};