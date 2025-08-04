#include "decl.h"

TranslationUnitDecl* TranslationUnitDecl::NewObj(const DeclGroup& dc)
{
    return new TranslationUnitDecl(dc);
}
void TranslationUnitDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

LabelDecl* LabelDecl::NewObj(Symbol* id)
{
    return new LabelDecl(id);
}
void LabelDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

ValueDecl* ValueDecl::NewObj(Symbol* id, QualType ty)
{
    return new ValueDecl(id, ty);
}
void ValueDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

DeclaratorDecl* DeclaratorDecl::NewObj(Symbol* id, QualType ty, int sc)
{
    return new DeclaratorDecl(id, ty, sc);
}
void DeclaratorDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

VarDecl* VarDecl::NewObj(Symbol* id, QualType ty, int sc, Expr* ex)
{
    return new VarDecl(id, ty, sc, ex);
}
void VarDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

ParmVarDecl* ParmVarDecl::NewObj(Symbol* id, QualType ty, int sc, Expr* ex)
{
    return new ParmVarDecl(id, ty, sc, ex);
}
void ParmVarDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

FieldDecl* FieldDecl::NewObj(Symbol* id, QualType ty, Decl* parent, unsigned offset)
{
    return new FieldDecl(id, ty, parent, offset);
}
void FieldDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

EnumConstantDecl* EnumConstantDecl::NewObj(Symbol* id, Expr* val)
{
    return new EnumConstantDecl(id, QualType(), val);
}
void EnumConstantDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

TypedefDecl* TypedefDecl::NewObj(Symbol* id, QualType ty)
{
    return new TypedefDecl(id, ty);
}
void TypedefDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

EnumDecl* EnumDecl::NewObj(Symbol* id, bool isDefinition)
{
    return new EnumDecl(id, isDefinition);  
}
void EnumDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

RecordDecl* RecordDecl::NewObj(Symbol* id, bool isDefinition, bool isUnion)
{
    return new RecordDecl(id, isDefinition, isUnion);     
}
void RecordDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}
