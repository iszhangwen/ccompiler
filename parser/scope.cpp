#include "scope.h"

Scope::Scope(ScopeType st, Scope* parent)
: st_(st), parent_(parent)
{

}

bool Scope::lookup(IdentifierInfo*, Decl*)
{
    return true;
}

bool Scope::insert(Decl*)
{
    return true;
}

Scope* Scope::getParent()
{
    return parent_;
}