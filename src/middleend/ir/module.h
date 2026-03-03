/*
表示一个编译单元
*/
#pragma once

#include <string>
#include <memory>
#include <list>

class GlobalVariable;
class Function;
class SymbolTable;

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
    // 符号表

private:
    std::string m_name;
    std::list<std::shared_ptr<Function>> m_functions;
    std::list<std::shared_ptr<GlobalVariable>> m_globalVars;
};