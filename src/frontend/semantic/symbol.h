#pragma once
#include <string>
#include <unordered_map>
#include "type.h"

class NamedDecl;
class Type;
class Scope;
class Token;

class Symbol 
{
public:
    // 名称空间规定了同一个名称空间的元素不能相同，即同一作用域不同名称空间的标识符名可以相同 
    // 同一个作用域必须要划分不同的名称空间，必然插入时开销
    enum NameSpace
    {
        LABEL, // 标签是独特的
        RECORD, // struct union enum共享同一个名称空间
        MEMBER, // struct union的成员是一个单独的名称空间
        NORMAL // 普通标识符的名称空间
    };

    Symbol()
    : m_nameSpace(NORMAL)
    , m_scope()
    , m_name("")
    , m_type(QualType())
    , m_decl()
    , m_isType(false){}

    static std::string getTag(NameSpace, const std::string&);
    std::string getTag();

    NameSpace getNameSpace() {return m_nameSpace;}
    void setNameSpace(NameSpace st) {m_nameSpace = st;}

    const std::string& getName() const {return m_name;}
    void setName(const std::string& key) {m_name = key;}

    Scope* getScope() {return m_scope;}
    void setScope(Scope* sc) {m_scope = sc;}

    QualType getType() { return m_type;}
    void setType(QualType ty) {m_type = ty;}

    NamedDecl* getDecl() { return m_decl;}
    void setDecl(NamedDecl* dc) {m_decl = dc;}

    bool isTypeName() {return m_isType;}

private:
    bool m_isType; // 是否是类型声明
    QualType m_type; // 符号的类型
    NameSpace m_nameSpace;
    std::string m_name;
    Scope* m_scope;
    NamedDecl* m_decl;
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
    int m_level; // 作用域深度
    Scope* m_parent;
    ScopeType m_scopeType;
    std::unordered_map<std::string, Symbol*> m_sysbolTable;

public:
    Scope(ScopeType st, Scope* parent);
    Scope* getParent() {return m_parent;}
    ScopeType getScopeType() {return m_scopeType;}
    int getLevel() const {return m_level;}
    Symbol* lookup(Symbol::NameSpace, const std::string&);
    bool insert(Symbol*);
    bool isFileScope() {return getScopeType() == ScopeType::FILE;}
    bool isBlockScope() {return !isFileScope();}
};

/*符号表管理
*/
class SymbolTableContext 
{
public:
    SymbolTableContext(): m_curScope(nullptr) {}
    // 符号查询和插入
    bool insertLabel(Symbol*);
    bool insertRecord(Symbol*);
    bool insertMember(Symbol*);
    bool insertNormal(Symbol*);
    Symbol* LookupLabel(const std::string&);
    Symbol* LookupRecord(const std::string&);
    Symbol* LookupMember(const std::string&);
    Symbol* LookupNormal(const std::string&);
    // 作用域管理函数
    void enterFileScope();
    void enterFuncScope();
    void enterBlockScope();
    void enterFuncPrototypeScope();
    void exitScope();
    Scope* getCurScope() {return m_curScope;}

    // 类型检测
    bool isTypeName(Token* tk);
    bool isTypeSpecifier(Token* tk);
    bool isTypeQualifier(Token* tk);

    // 设置内置类型
    void initBuiltType();
    // 获取内置类型
    Type* getBuiltTypeByTS(int);

private:
    Scope* m_curScope;
};
