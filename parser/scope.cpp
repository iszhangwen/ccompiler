#include "scope.h"

bool Scope::lookup(const std::string& name, std::shared_ptr<Identifier>& ident)
{
    if (sysbol_.count(name) == 0) {
        return false;
    }
    ident = sysbol_[name];
    return true;
}

bool Scope::insert(const std::string& name, std::shared_ptr<Identifier> ident)
{
    if (sysbol_.count(name) == 0) {
        sysbol_[name] = ident;
        return true;
    }
    return false;
}