#include "expr.h"

void DeclRefExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void IntegerLiteral::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void CharacterLiteral::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void FloatingLiteral::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void StringLiteral::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void ParenExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void UnaryOpExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void ArraySubscriptExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void CallExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void MemberExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void CompoundLiteralExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void CastExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void BinaryOpExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

void ConditionalExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}


