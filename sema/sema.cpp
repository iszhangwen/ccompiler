#include "sema.h"
#include <ast/decl.h>
#include <ast/expr.h>
#include <ast/stmt.h>
#include <ast/type.h>

TranslationUnitDecl* sema::onActTranslationUnit(Scope* sc, std::vector<Decl*>& vec)
{
    return new TranslationUnitDecl(vec);
}

NamedDecl* sema::onActFunctionDecl()
{
    return nullptr;
}

NamedDecl* sema::onActNamedDecl()
{
    return nullptr;
}

// create a qual Type
Type* sema::onActType(int ts)
{
    switch (ts)
    {
    case TypeSpecifier::VOID:
        VoidTy
    
    default:
        break;
    }
}