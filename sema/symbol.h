#pragma once
#include <string>
#include <unordered_map>
#include <ast/type.h>

class NamedDecl;
class Type;
class Scope;
class Token;

class Symbol
{
public:
    // 名称空间规定了同一个名称空间的元素不能相同，即同一作用域不同名称空间的标识符名可以相同 
    // 同一个作用域必须要划分不同的名称空间，必然插入时开销
    enum SymbolType
    {
        LABEL, // 标签是独特的
        RECORD, // struct union enum共享同一个名称空间
        MEMBER, // struct union的成员是一个单独的名称空间
        NORMAL // 普通标识符的名称空间
    };

private:
    SymbolType st_;
    std::string key_;
    Scope* sc_;
    QualType qty_;
    NamedDecl* dc_;
    bool isType_; // 是否是类型声明
protected:
    Symbol(SymbolType st, Scope* s, const std::string& k, QualType t, NamedDecl* dc, bool isType = false)
    : st_(st), sc_(s), key_(k), qty_(t), dc_(dc), isType_(isType){}

public:
    static Symbol* NewObj(SymbolType st, Scope* s, const std::string& k, QualType t, NamedDecl* dc, bool isType = false);
    static std::string getTag(SymbolType, const std::string&);
    std::string getTag();

    const std::string& getKey() const {return key_;}
    void setKey(const std::string& key) {key_ = key;}

    Scope* getScope() {return sc_;}
    void setScope(Scope* sc) {sc_ = sc;}

    QualType getType() { return qty_;}
    void setType(QualType ty) {qty_ = ty;}

    NamedDecl* getDecl() { return dc_;}
    void setDecl(NamedDecl* dc) {dc_ = dc;}

    bool isTypeName() {return isType_;}
};

// 声明上下文，使用声明上下文代替了作用域和符号表 
class Scope
{
public:
    // 相同的标识符要么具有不同的名称空间，要么具有不同的作用域。
    // 作用域有四种：函数，文件，块和函数原型
    enum ScopeType {
        FILE, 
        FUNC,  // label是唯一的函数作用域标识符
        BLOCK, // 块或者函数定义的参数声明列表
        FUNC_PROTOTYPE // 函数原型(即函数声明)的参数声明列表
    };

private:
    int level_; // 作用域深度
    Scope* parent_;
    ScopeType sct_;
    std::unordered_map<std::string, Symbol*> table_;

public:
    Scope(ScopeType st, Scope* parent);
    Scope* getParent() {return parent_;}
    ScopeType getScopeType() {return sct_;}
    int getLevel() const {return level_;}
    Symbol* lookup(Symbol::SymbolType, const std::string&);
    bool insert(Symbol*); 
};

/*符号表管理
*/
class SymbolTableContext {
    Scope* curScope_;
    Symbol* insert(Symbol::SymbolType, const std::string&, QualType, NamedDecl*, bool isType = false);
    Symbol* insert(Symbol::SymbolType, const std::string&, Type*, NamedDecl*, bool isType = false);
public:
    SymbolTableContext(): curScope_(nullptr) {}
    // 符号查询和插入
    Symbol* insertLabel(const std::string&, NamedDecl*);
    Symbol* insertRecord(const std::string&, QualType, NamedDecl*);
    Symbol* insertMember(const std::string&, QualType, NamedDecl*);
    Symbol* insertNormal(const std::string&, QualType, NamedDecl*, bool isType = false);
    Symbol* lookup(Symbol::SymbolType, const std::string&);
    // 作用域管理函数
    Scope* getCurScope() {return curScope_;}
    void enterScope(Scope::ScopeType);
    void exitScope();
    bool isScopeType(Scope::ScopeType st) {return curScope_->getScopeType() == st;}

    // 类型检测
    bool isTypeName(Token* tk);
    bool isTypeSpecifier(Token* tk);
    bool isTypeQualifier(Token* tk);

    // 设置内置类型
    void initBuiltType();
    // 获取内置类型
    QualType getBuiltTypeByTypeSpec(int);
};