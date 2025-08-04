#include "symbol.h"
#include <ast/decl.h>

Symbol* Symbol::NewObj(SymbolType st, Scope* s, const std::string& k, Type* t, NamedDecl* dc, bool isType)
{
    return new Symbol(st, s, k, t, dc, isType);
}

std::string Symbol::getTag() 
{
    return getTag(st_, key_);
}

std::string Symbol::getTag(SymbolType st, const std::string& key)
{
    std::string tail;
    switch (st)
    {
    case LABEL:
        tail.append("@LABEL");
        break;
    case RECORD:
        tail.append("@RECORD");
        break;
    case MEMBER:
        tail.append("@RECORD_MEMBER");
        break;
    case NORMAL:
    default:
        tail.append("@NORMAL");
        break;
    }
    return key + tail;
}

Scope::Scope(ScopeType st, Scope* parent)
: sct_(st), parent_(parent)
{
    level_ = parent ? 1 : (parent->getLevel() + 1);
}

Scope* Scope::NewObj(ScopeType st, Scope* parent)
{
    return new Scope(st, parent);
}

Symbol* Scope::lookup(Symbol::SymbolType st, const std::string& key)
{
    std::string tmp = Symbol::getTag(st, key);
    if (table_.count(tmp)) {
        return table_[tmp];
    }
    if (!parent_) {
        return parent_->lookup(st, key);
    }
    return nullptr;
}

bool Scope::insert(Symbol* sy)
{
    if (!sy) {
        return false;
    }
    std::string tmp = sy->getTag();
    if (!table_.count(tmp)) {
        table_[tmp] = sy;
        return true;
    }
    return false;
}

Symbol* SymbolTableContext::insert(Symbol::SymbolType st, const std::string& key, Type* ty, NamedDecl* dc)
{
    if (!curScope_) {
        return nullptr;
    }
    Symbol* sym = Symbol::NewObj(st, curScope_, key, ty, dc);
    if (!sym) {
        return nullptr;
    }
    if (!curScope_->insert(sym)) {
        delete sym;
        return nullptr;
    }
    return sym;
} 

Symbol* SymbolTableContext::insertLabel(const std::string& key, NamedDecl* dc)
{
    return insert(Symbol::LABEL, key, nullptr, dc);
}

Symbol* SymbolTableContext::insertRecord(const std::string& key, Type* ty, NamedDecl* dc)
{
    return insert(Symbol::RECORD, key, ty, dc);
}

Symbol* SymbolTableContext::insertMember(const std::string& key, Type* ty, NamedDecl* dc)
{
    return insert(Symbol::MEMBER, key, ty, dc);
}

Symbol* SymbolTableContext::insertNormal(const std::string& key, Type* ty, NamedDecl* dc)
{
    return insert(Symbol::NORMAL, key, ty, dc);
}

Symbol* SymbolTableContext::lookup(Symbol::SymbolType st, const std::string& key)
{
    return curScope_->lookup(st, key);
}  

void SymbolTableContext::enterScope(Scope::ScopeType st)
{
    curScope_ = Scope::NewObj(st, curScope_);
}

void SymbolTableContext::exitScope()
{
    if (!curScope_) {
        throw std::string("curScope_ is nullptr");
    }
    curScope_ = curScope_->getParent();
}

/*
(6.7.7) type-name:
 specifier-qualifier-list abstract-declaratoropt
  (6.7.2.1) specifier-qualifier-list:
 type-specifier specifier-qualifier-listopt
 type-qualifier specifier-qualifier-listopt
*/
bool SymbolTableContext::isTypeName(Token* tk)
{
    if (!tk) {
        return false;
    }
    return (isTypeSpecifier(tk) || isTypeQualifier(tk));
}

bool SymbolTableContext::isTypeSpecifier(Token* tk)
{
    if (!tk) {
        return false;
    }
    // 判断是否是类型说明
    switch (tk->kind_)
    {
    case TokenKind::Void:
    case TokenKind::Char:
    case TokenKind::Short:
    case TokenKind::Int:
    case TokenKind::Long:
    case TokenKind::Float:
    case TokenKind::Double:
    case TokenKind::Signed:
    case TokenKind::Unsigned:
    case TokenKind::T_Bool:
    case TokenKind::T_Complex:
    /* atomic-type-specifier*/
    /*struct-or-union-specifier*/
    case TokenKind::Struct:
    case TokenKind::Union:
    /*enum-specifier*/
    case TokenKind::Enum:
        return true;
    default:
        break;
    }
    /*typedef-name*/
    Symbol* sym = lookup(Symbol::NORMAL, tk->value_);
    if (sym) {
        return true;
    }
    return false;
}

bool SymbolTableContext::isTypeQualifier(Token* tk)
{
    if (!tk) {
        return false;
    }
    // 判断是否是类型限定符
    switch (tk->kind_)
    {
    case TokenKind::Const:
    case TokenKind::Volatile:
    case TokenKind::Restrict:
    case TokenKind::T_Atomic:
        return true;
    default:
        break;
    }
    return false;
}   