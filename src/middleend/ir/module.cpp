#include "module.h"
#include "value.h"
#include "irvisitor.h"

Value* Module::getGlobalDeclAddr(NamedDecl* decl)
{
    if (m_globalDeclAddr.count(decl)) 
        return m_globalDeclAddr[decl];
    return nullptr;
}

void  Module::setGlobalDeclAddr(NamedDecl* decl, Value* val)
{
    if (m_globalDeclAddr.count(decl)) 
        return;
    m_globalDeclAddr[decl] = val;
}

std::any Module::accept(IRVisitor* vt)
{
    if (vt) {
        return vt->visit(this);
    }
    return std::any();
}