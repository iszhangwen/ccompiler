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

IfStmt* IfStmt::NewObj(Expr* cond, Stmt* th, Stmt* el)
{
    return new IfStmt(cond, th, el);
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

WhileStmt* WhileStmt::NewObj(Expr* cond, Stmt* val)
{
    return new WhileStmt(cond, val);
}
void WhileStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

DoStmt* DoStmt::NewObj(Expr* cond, Stmt* val)
{
    return new DoStmt(cond, val);
}
void DoStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

ForStmt* ForStmt::NewObj(Expr* init, Expr* cond, Expr* update, Stmt* val)
{
    return new ForStmt(init, cond, update, val);
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

ContinueStmt* ContinueStmt::NewObj(Stmt* label)
{
    return new ContinueStmt(label);
}
void ContinueStmt::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

BreakStmt* BreakStmt::NewObj(Stmt* label)
{
    return new BreakStmt(label);
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







