#include "symbol.h"
#include <ast/decl.h>

Symbol* Symbol::NewObj(SymbolType st, Scope* s, const std::string& k, Type* t, NamedDecl* dc)
{
    return new Symbol(st, s, k, t, dc);
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