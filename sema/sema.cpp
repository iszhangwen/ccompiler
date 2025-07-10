#include "sema.h"
#include <ast/decl.h>
#include <ast/expr.h>
#include <ast/stmt.h>
#include <ast/type.h>

TranslationUnitDecl* sema::onActTranslationUnit(Scope* sc, std::vector<Decl*>& vec)
{
    return new TranslationUnitDecl(vec);
}

FunctionDecl* sema::onActFunctionDecl()
{
    return nullptr;
}

VarDecl* sema::onActVarDecl()
{
    return nullptr;
}

VarDecl* sema::onActArrayDecl()
{
    return nullptr;
}

// create a qual Type
Type* sema::onActBuiltinType(int ts)
{
    return nullptr;
}

Type* sema::onActPointerType(QualType& qt)
{
    return PointerType::newObj(qt);
}