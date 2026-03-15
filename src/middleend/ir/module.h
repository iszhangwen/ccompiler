/*
表示一个编译单元
*/
#pragma once

#include "usedef.h"

#include <string>
#include <memory>
#include <list>
#include <unordered_map>

class GlobalVariable;
class Function;
class LValue;

class Module : public ArenaNode<Module>
{
public:
    explicit Module(const std::string& name)
    : m_name(name){}
    ~Module(){}

    // 模块名称
    void setName(const std::string& name) {m_name = name;}
    std::string getName() {return m_name;}
    // 函数操作
    void addFunction(Function* func) {m_functions.push_back(func);}
    std::list<Function*> getFunctions() {return m_functions;}
    // 全局变量
    void addGlobalVar(GlobalVariable* var);
    std::list<GlobalVariable*> getGlobalVars() {return m_globalVars;}

    // 全局声明地址获取：nameDecl存储作用域和符号表信息
    Value* getGlobalDeclAddr(NamedDecl* decl);
    void setGlobalDeclAddr(NamedDecl* decl, Value* val);

private:
    std::string m_name;
    std::list<Function*> m_functions;
    std::list<GlobalVariable*> m_globalVars;
    std::unordered_map<NamedDecl*, Value*> m_globalDeclAddr;
};