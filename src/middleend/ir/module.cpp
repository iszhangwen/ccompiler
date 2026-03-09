#include "module.h"
#include "value.h"

std::shared_ptr<Value> Module::getGlobalDeclAddr(NamedDecl* decl)
{
    if (m_globalDeclAddr.count(decl)) 
        return m_globalDeclAddr[decl];
    return nullptr;
}

void  Module::setGlobalDeclAddr(NamedDecl* decl, std::shared_ptr<Value> val)
{
    if (m_globalDeclAddr.count(decl)) 
        return;
    m_globalDeclAddr[decl] = val;
}
