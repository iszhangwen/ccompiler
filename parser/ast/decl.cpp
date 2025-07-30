#include "decl.h"

TranslationUnitDecl* TranslationUnitDecl::NewObj(const DeclGroup& dc)
{
    return new TranslationUnitDecl(dc);
}

LabelDecl* LabelDecl::NewObj(Declarator id)
{
    return new LabelDecl(id);
}

ValueDecl* ValueDecl::NewObj(Declarator id)
{
    return new ValueDecl(id);
}

DeclaratorDecl* DeclaratorDecl::NewObj(Declarator id)
{
    return new DeclaratorDecl(id);
}

VarDecl* VarDecl::NewObj(Symbol* id, QualType ty, int sc, Expr* ex)
{
    return new VarDecl(id, ty, sc, ex);
}

ParmVarDecl* ParmVarDecl::NewObj(Symbol* id, QualType ty, int sc, Expr* ex)
{
    return new ParmVarDecl(id, ty, sc, ex);
}

FieldDecl* FieldDecl::NewObj(Symbol* id, QualType ty, Decl* parent, unsigned offset)
{
    return new FieldDecl(id, ty, parent, offset);
}

EnumConstantDecl* EnumConstantDecl::NewObj(Symbol* id, Expr* val)
{
    return new EnumConstantDecl(id, QualType(), val);
}

TypedefDecl* TypedefDecl::NewObj(Symbol* id, QualType ty)
{
    return new TypedefDecl(id, ty);
}

EnumDecl* EnumDecl::NewObj(Symbol* id, bool isDefinition)
{
    return new EnumDecl(id, isDefinition);  
}

RecordDecl* RecordDecl::NewObj(Symbol* id, bool isDefinition, bool isUnion)
{
    return new RecordDecl(id, isDefinition, isUnion);     
}
