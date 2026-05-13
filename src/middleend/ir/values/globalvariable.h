#pragma once

#include "usedef.h"
#include "globalvalue.h"

namespace ccompiler {

class GlobalVariable : public GlobalValue
{
public:
    GlobalVariable(QualType ty, const std::string& name, Module* parent = nullptr)
    : GlobalValue(ty, name, parent){}
};
} // namespace ccompiler
