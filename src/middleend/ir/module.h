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
class IRVisitor;

class Module 
{
public:
    explicit Module(const std::string& name = "")
    : m_name(name){}
    ~Module(){}

    // @brief: 设置模块名称
    void setName(const std::string& name) {m_name = name;}
    // @brief: 获取模块名称
    std::string getName() const {return m_name;}

    // @brief: 添加函数
    void addFunction(Function* func) {m_functions.push_back(func);}
    // @brief: 获取模块所有的函数
    std::list<Function*> getFunctions() {return m_functions;}

    // @brief: 设置当前编译的函数
    void setCurFunction(Function* val) {m_curFunction = val;}
    // @brief: 获取当前编译的函数
    Function* getCurFunction() {return m_curFunction;}

    // @brief: 全局变量
    void addGlobalVar(GlobalVariable* var) {m_globalVars.push_back(var);}
    std::list<GlobalVariable*> getGlobalVars() {return m_globalVars;}

    // 全局声明地址获取：nameDecl存储作用域和符号表信息
    Value* getGlobalDeclAddr(NamedDecl* decl);
    void addGlobalDeclAddr(NamedDecl* decl, Value* val);

    // @brief: 创建函数
    Function* createFunctionIR(QualType ty, const std::string& name);
    // @brief: 创建全局变量并自动添加到容器中
    GlobalVariable* createGlobalVar(QualType ty, const std::string& name);

    // @brief: 打印出IR
    void toStringPrint();

private:
    std::string m_name;
    Function* m_curFunction;
    std::list<Function*> m_functions;
    std::list<GlobalVariable*> m_globalVars;
    std::unordered_map<NamedDecl*, Value*> m_globalDeclAddr;
};
