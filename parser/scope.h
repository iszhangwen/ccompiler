#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>
#include "token.h"

// 标识符信息： 标识符有两个重要属性：作用域和生命周期
struct IdentifierInfo
{
    std::string name;
    TokenKind kind;
    IdentifierInfo(){}
    IdentifierInfo(Token tk): name(tk.getValue()), kind(tk.getKind()){}
};

// 标识符要么具有不同的名称空间，要么具有不同的作用域。
// 名称空间分为：
// 
// 作用域有四种：函数，文件，块和函数原型
enum class ScopeType {
    FILE, 
    FUNC,  // label是唯一的函数作用域标识符
    BLOCK, // 块或者函数定义的参数声明列表
    FUNC_PROTOTYPE // 函数原型(即函数声明)的参数声明列表
};

// 名称空间规定了同一个名称空间的元素不能相同，即同一作用域不同名称空间的标识符名可以相同 
// 同一个作用域必须要划分不同的名称空间，必然插入时开销
enum class NameSpace
{
    LABEL, // 标签是独特的
    RECORD, // struct union enum共享同一个名称空间
    RECORD_MEMBER, // struct union的成员是一个单独的名称空间
    NORMAL // 普通标识符的名称空间
};

class Scope {
    
    using SymbolTable = std::unordered_map<std::string, std::shared_ptr<IdentifierInfo>>;
public:
    explicit Scope(std::shared_ptr<Scope> parent, ScopeType type):parent_(parent), type_(type){}
    ~Scope(){}

    // 设置开作用域范围
    void setParentScope(std::shared_ptr<Scope> parent) {
        parent_ = parent;
    }

    // 设置作用域类型
    void setScopeType(ScopeType type) {
        type_ = type;
    }

    // 查找符号
    bool lookup(const std::string& name, std::shared_ptr<IdentifierInfo>&);

    // 插入符号
    bool insert(const std::string& name, std::shared_ptr<IdentifierInfo>);

    // 遍历符号
    int count() const {
        return sysbol_.size();
    }
    void print() const {
        std::cout << "---start print new scope iden-----\n";
        for (auto& [key, _] : sysbol_) {
            std::cout << key << "\n";
        }
        std::cout << "--------------end-----------------\n";
    }
private:
    ScopeType type_;
    SymbolTable sysbol_;
    std::shared_ptr<Scope> parent_;
};