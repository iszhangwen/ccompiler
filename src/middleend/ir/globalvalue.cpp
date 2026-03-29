#include "globalvalue.h"
#include "function.h"
#include "values/globalvariable.h"

GlobalValue::GlobalValue(QualType ty, const std::string& name, Module* parent)
    : User(ty, name), m_parent(parent)
{
    // 默认使用4字节对齐
    m_alignment = 4;
    // 默认可见
    m_visibility = DefaultVisibility;
    // 默认内部链接
    m_linkType = InternalType;
}

bool GlobalValue::isFunction()
{
    return isa<Function>() != nullptr;
}

bool GlobalValue::isGlobalVariable()
{
    return isa<GlobalVariable>() != nullptr;
}
