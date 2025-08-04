#include "expr.h"

DeclRefExpr* DeclRefExpr::NewObj(QualType qt, NamedDecl* dc)
{
    return new DeclRefExpr(qt, dc);
}
void DeclRefExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

IntegerLiteral* IntegerLiteral::NewObj(Token* tk)
{
    return new IntegerLiteral(QualType(), tk->value_);
}
void IntegerLiteral::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

CharacterLiteral* CharacterLiteral::NewObj(Token* tk)
{
    return new CharacterLiteral(QualType(), tk->value_);
}
void CharacterLiteral::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

FloatingLiteral* FloatingLiteral::NewObj(Token* tk)
{
    return new FloatingLiteral(QualType(), tk->value_);
}
void FloatingLiteral::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

StringLiteral* StringLiteral::NewObj(Token* tk)
{
    return new StringLiteral(QualType(), tk->value_);
}
void StringLiteral::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

ParenExpr* ParenExpr::NewObj(Expr* val)
{
    return new ParenExpr(val);
}
void ParenExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

UnaryOpExpr* UnaryOpExpr::NewObj(Expr* val, OpCode op)
{
    return new UnaryOpExpr(QualType(), val, op);
}
UnaryOpExpr* UnaryOpExpr::NewObj(QualType qt, Expr* val, OpCode op)
{
    return new UnaryOpExpr(qt, val, op);
}
void UnaryOpExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

ArraySubscriptExpr* ArraySubscriptExpr::NewObj(Expr* base, Expr* index)
{
    return new ArraySubscriptExpr(base, index);
}
void ArraySubscriptExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

CallExpr* CallExpr::NewObj(Expr* fn, const std::vector<Expr*>& params)
{
    return new CallExpr(fn, params);
}
void CallExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

MemberExpr* MemberExpr::NewObj(Expr* base, bool isArrow, NamedDecl* member, QualType qt)
{
    return new MemberExpr(base, isArrow, member, qt);
}
void MemberExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

CompoundLiteralExpr* CompoundLiteralExpr::NewObj(QualType qt, Expr* ex)
{
    return new CompoundLiteralExpr(qt, ex);
}
void CompoundLiteralExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

CastExpr* CastExpr::NewObj(QualType qt, Expr* ex, CastKind ck)
{
    return new CastExpr(qt, ex, ck);
}
void CastExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

BinaryOpExpr* BinaryOpExpr::NewObj(Expr* LE, Expr* RE, OpCode op)
{
    return new BinaryOpExpr(LE, RE, op, QualType());
}
BinaryOpExpr* BinaryOpExpr::NewObj(Expr* LE, Expr* RE, OpCode op, QualType qt)
{
    return new BinaryOpExpr(LE, RE, op, qt);
}
void BinaryOpExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}

ConditionalExpr* ConditionalExpr::NewObj(Expr* co, Expr* th, Expr* el)
{
    return new ConditionalExpr(co, th, el, QualType());
}
ConditionalExpr* ConditionalExpr::NewObj(Expr* co, Expr* th, Expr* el, QualType qt)
{
    return new ConditionalExpr(co, th, el, qt);
}
void ConditionalExpr::accept(ASTVisitor* vt) 
{
    if (vt) {
        vt->visit(this);
    }
}


