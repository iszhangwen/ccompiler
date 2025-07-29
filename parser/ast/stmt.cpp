#include "stmt.h"

LabelStmt* LabelStmt::NewObj(NamedDecl* key, Stmt* val)
{
    return new LabelStmt(key, val);
}

CaseStmt* CaseStmt::NewObj(Expr* cond, Stmt* val)
{
    return new CaseStmt(nullptr, val);
}

DefaultStmt* DefaultStmt::NewObj(Expr* cond, Stmt* val)
{
    return new DefaultStmt(nullptr, val);
}

CompoundStmt* CompoundStmt::NewObj(const std::vector<Stmt*>& vals)
{
    return new CompoundStmt(vals);
}

DeclStmt* DeclStmt::NewObj(Decl* dc)
{
    return new DeclStmt(dc);
}

ExprStmt* ExprStmt::NewObj(Expr* ex)
{
    return new ExprStmt(ex);
}

IFStmt* IFStmt::NewObj(Expr* cond, Stmt* th, Stmt* el)
{
    return new IFStmt(cond, th, el);
}

SwitchStmt* SwitchStmt::NewObj(Expr* cond, Stmt* val)
{
    return new SwitchStmt(cond, val);
}

WhileStmt* WhileStmt::NewObj(Expr* cond, Stmt* val)
{
    return new WhileStmt(cond, val);
}

DoStmt* DoStmt::NewObj(Expr* cond, Stmt* val)
{
    return new DoStmt(cond, val);
}

ForStmt* ForStmt::NewObj(Expr* init, Expr* cond, Expr* update, Stmt* val)
{
    return new ForStmt(init, cond, update, val);
}

GotoStmt* GotoStmt::NewObj(Stmt* label)
{
    return new GotoStmt(label);
}

ContinueStmt* ContinueStmt::NewObj(Stmt* label)
{
    return new ContinueStmt(label);
}

BreakStmt* BreakStmt::NewObj(Stmt* label)
{
    return new BreakStmt(label);
}

ReturnStmt* ReturnStmt::NewObj(Expr* retVal)
{
    return new ReturnStmt(retVal);
}







