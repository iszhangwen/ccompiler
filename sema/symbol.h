#pragma once
#include <string>
#include <unordered_map>

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
    Type* ty_;
    NamedDecl* dc_;

protected:
    Symbol(SymbolType st, Scope* s, const std::string& k, Type* t, NamedDecl* dc)
    : st_(st), sc_(s), key_(k), ty_(t), dc_(dc) {}

public:
    static Symbol* NewObj(SymbolType st, Scope* s, const std::string& k, Type* t, NamedDecl* dc);
    static std::string getTag(SymbolType, const std::string&);
    std::string getTag();

    const std::string& getKey() const {return key_;}
    void setKey(const std::string& key) {key_ = key;}

    Scope* getScope() {return sc_;}
    void setScope(Scope* sc) {sc_ = sc;}

    Type* getType() { return ty_;}
    void setType(Type* ty) {ty_ = ty;}

    NamedDecl* getNamedDecl() { return dc_;}
    void setNamedDecl(NamedDecl* dc) {dc_ = dc;}
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
    Scope(ScopeType st, Scope* parent);

public:
    static Scope* NewObj(ScopeType st, Scope* parent);
    Scope* getParent() {return parent_;}
    int getLevel() const {return level_;}
    Symbol* lookup(Symbol::SymbolType, const std::string&);
    bool insert(Symbol*); 
};

/*符号表管理
*/
class SymbolTableContext {
    Scope* curScope_;
    Symbol* insert(Symbol::SymbolType, const std::string&, Type*, NamedDecl*);
public:
    SymbolTableContext(): curScope_(nullptr) {}
    // 符号查询和插入
    Symbol* insertLabel(const std::string&, NamedDecl*);
    Symbol* insertRecord(const std::string&, Type*, NamedDecl*);
    Symbol* insertMember(const std::string&, Type*, NamedDecl*);
    Symbol* insertNormal(const std::string&, Type*, NamedDecl*);
    Symbol* lookup(Symbol::SymbolType, const std::string&);
    // 作用域管理函数
    Scope* getCurScope() {return curScope_;}
    void enterScope(Scope::ScopeType);
    void exitScope();

    // 类型检测
    bool isTypeName(Token*);
};