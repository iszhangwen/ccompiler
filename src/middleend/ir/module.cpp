#include "module.h"
#include "function.h"
#include "values/globalvariable.h"

#include <iostream>

Value* Module::getGlobalDeclAddr(NamedDecl* decl)
{
    if (m_globalDeclAddr.count(decl)) 
        return m_globalDeclAddr[decl];
    return nullptr;
}

void Module::addGlobalDeclAddr(NamedDecl* decl, Value* val)
{
    if (m_globalDeclAddr.count(decl)) 
        return;
    m_globalDeclAddr[decl] = val;
}

Function* Module::createFunctionIR(QualType ty, const std::string& name)
{
    // 创建function ir对象
    auto newFunc = Arena::make<Function>(ty, name, this);
    if (!newFunc) {
        return nullptr;
    }
    // 将函数对象加入到连表
    addFunction(newFunc);
    return newFunc;
}

GlobalVariable* Module::createGlobalVar(QualType ty, const std::string& name)
{
    // 创建SSA值对象
    auto var = Arena::make<GlobalVariable>(ty, name, this);
    if (!var) {
        return nullptr;
    }
    // 将全局变量加入到列表
    addGlobalVar(var);
    return var;
}

void Module::toStringPrint()
{
    for (auto func : getFunctions()) {
        std::cout << "entry function: "  << func->getName() << "\n";
        func->toStringPrint();
        std::cout << "exit\n";
    }
}
