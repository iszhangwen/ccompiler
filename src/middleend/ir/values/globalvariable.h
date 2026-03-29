#pragma once

#include "usedef.h"
#include "globalvalue.h"

class GlobalVariable : public GlobalValue
{
public:
    GlobalVariable(QualType ty, const std::string& name, Module* parent = nullptr)
    : GlobalValue(ty, name, parent){}
};
