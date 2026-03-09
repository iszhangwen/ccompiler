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

class Module
{
public:
    explicit Module(const std::string& name)
    : m_name(name){}
    ~Module(){}

    // 模块名称
    void setName(const std::string& name) {m_name = name;}
    std::string getName() {return m_name;}
    // 函数类
    void addFunction(std::shared_ptr<Function> func) {m_functions.push_back(func);}
    std::list<std::shared_ptr<Function>> getFunctions() {return m_functions;}
    // 全局变量
    void addGlobalVar(std::shared_ptr<GlobalVariable> var);
    std::list<std::shared_ptr<GlobalVariable>> getGlobalVars() {return m_globalVars;}

    // 全局声明地址获取：nameDecl存储作用域和符号表信息
    std::shared_ptr<Value> getGlobalDeclAddr(NamedDecl* decl);
    void setGlobalDeclAddr(NamedDecl* decl, std::shared_ptr<Value> val);

private:
    std::string m_name;
    std::list<std::shared_ptr<Function>> m_functions;
    std::list<std::shared_ptr<GlobalVariable>> m_globalVars;
    std::unordered_map<NamedDecl*, std::shared_ptr<Value>> m_globalDeclAddr;
};