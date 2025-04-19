#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

using Identifier = int;

enum class ScopeType {
    FUNC,
    FILE,
    BLOCK
};

class Scope {
    
    using SymbolTable = std::unordered_map<std::string, std::shared_ptr<Identifier>>;
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
    bool lookup(const std::string& name, std::shared_ptr<Identifier>&);

    // 插入符号
    bool insert(const std::string& name, std::shared_ptr<Identifier>);

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