#include "stmt.h"

void LabelStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void CaseStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void DefaultStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void CompoundStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
    for (auto body : m_bodys) {
        body->accept(vt);
    }
}

void DeclStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void ExprStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void IfStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void SwitchStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void WhileStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void DoStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void ForStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void GotoStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void ContinueStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void BreakStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void ReturnStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}







