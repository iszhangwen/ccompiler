#include "symbol.h"
#include <ast/decl.h>

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
    case RECORD_MEMBER:
        tail.append("@RECORD_MEMBER");
        break;
    case NORMAL:
    default:
        tail.append("@NORMAL");
        break;
    }
    return key + tail;
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
    if (sy == nullptr) {
        return false;
    }
    std::string tmp = sy->getTag();
    if (!table_.count(tmp)) {
        table_[tmp] = sy;
        return true;
    }
    return false;
}

void SymbolTableContext::enterScope(Scope::ScopeType st)
{
    curScope_ = new Scope(st, curScope_);
}

void SymbolTableContext::exitScope()
{
    if (!curScope_) {
        throw std::string("curScope_ is nullptr");
    }
    curScope_ = curScope_->getParent();
}

bool SymbolTableContext::insert(Symbol* sy)
{
    if (!curScope_) {
        return false;
    }
    return curScope_->insert(sy);
}   

bool SymbolTableContext::insert(SymbolType st, const std::string& k, Type* t)
{
    Symbol* sy = new Symbol(st, curScope_, k, t);
    return insert(sy);
} 

Symbol* SymbolTableContext::lookup(Symbol::SymbolType st, const std::string& key)
{
    return curScope_->lookup(st, key);
}

