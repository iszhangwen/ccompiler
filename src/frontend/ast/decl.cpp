#include "decl.h"
#include "stmt.h"

void TranslationUnitDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
    for (auto body : m_bodys)
    {
        body->accept(vt);
    }
}

void LabelDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void ValueDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void DeclaratorDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void VarDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void ParmVarDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void FunctionDecl::accept(ASTVisitor* vt)
{
    if (vt) {
        vt->visit(this);
    }
    m_body->accept(vt);
}

void FieldDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void EnumConstantDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void TypedefDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void EnumDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void RecordDecl::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}
