#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include <token.h>
#include <ast/decl.h>

// 标识符作为符号表对象，严格限制标识符只有名字和命名空间两个属性
class IdentifierInfo
{
public:
    std::string name;
};

// 声明上下文，使用声明上下文代替了作用域和符号表 
class Scope
{
public:
    // 标识符要么具有不同的名称空间，要么具有不同的作用域。
    // 作用域有四种：函数，文件，块和函数原型
    enum ScopeType {
        FILE, 
        FUNC,  // label是唯一的函数作用域标识符
        BLOCK, // 块或者函数定义的参数声明列表
        FUNC_PROTOTYPE // 函数原型(即函数声明)的参数声明列表
    };

    // 名称空间规定了同一个名称空间的元素不能相同，即同一作用域不同名称空间的标识符名可以相同 
    // 同一个作用域必须要划分不同的名称空间，必然插入时开销
    enum NameSpace
    {
        LABEL, // 标签是独特的
        RECORD, // struct union enum共享同一个名称空间
        RECORD_MEMBER, // struct union的成员是一个单独的名称空间
        NORMAL // 普通标识符的名称空间
    };

private:
    using SymbolTable = std::unordered_map<IdentifierInfo*, Decl*>;
    SymbolTable table_;

    Scope* parent_;
    ScopeType st_;

public:
    Scope(ScopeType st, Scope* parent);
    bool lookup(IdentifierInfo*, Decl*);
    bool insert(Decl*); 

    Scope* getParent();
};