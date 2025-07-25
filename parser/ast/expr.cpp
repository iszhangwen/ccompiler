#include "expr.h"

DeclRefExpr* DeclRefExpr::NewObj(QualType qt, NamedDecl* dc)
{
    return new DeclRefExpr(qt, dc);
}

IntegerLiteral* IntegerLiteral::NewObj(Token* tk)
{
    return new IntegerLiteral(QualType(), tk->value_);
}

CharacterLiteral* CharacterLiteral::NewObj(Token* tk)
{
    return new CharacterLiteral(QualType(), tk->value_);
}

FloatingLiteral* FloatingLiteral::NewObj(Token* tk)
{
    return new FloatingLiteral(QualType(), tk->value_);
}

StringLiteral* StringLiteral::NewObj(Token* tk)
{
    return new StringLiteral(QualType(), tk->value_);
}

ParenExpr* ParenExpr::NewObj(Expr* val)
{
    return new ParenExpr(val);
}

UnaryOpExpr* UnaryOpExpr::NewObj(Expr* val, OpCode op)
{
    return new UnaryOpExpr(QualType(), val, op);
}

UnaryOpExpr* UnaryOpExpr::NewObj(QualType qt, Expr* val, OpCode op)
{
    return new UnaryOpExpr(qt, val, op);
}

ArraySubscriptExpr* ArraySubscriptExpr::NewObj(Expr* base, Expr* index)
{
    return new ArraySubscriptExpr(base, index);
}

CallExpr* CallExpr::NewObj(Expr* fn, const std::vector<Expr*>& params)
{
    return new CallExpr(fn, params);
}

MemberExpr* MemberExpr::NewObj(Expr* base, bool isArrow, NamedDecl* member, QualType qt)
{
    return new MemberExpr(base, isArrow, member, qt);
}

CompoundLiteralExpr* CompoundLiteralExpr::NewObj(QualType qt, Expr* ex)
{
    return new CompoundLiteralExpr(qt, ex);
}

CastExpr* CastExpr::NewObj(QualType qt, Expr* ex, CastKind ck)
{
    return new CastExpr(qt, ex, ck);
}

BinaryOpExpr* BinaryOpExpr::NewObj(Expr* LE, Expr* RE, OpCode op)
{
    return new BinaryOpExpr(LE, RE, op, QualType());
}

BinaryOpExpr* BinaryOpExpr::NewObj(Expr* LE, Expr* RE, OpCode op, QualType qt)
{
    return new BinaryOpExpr(LE, RE, op, qt);
}

ConditionalOperator* ConditionalOperator::NewObj(Expr* co, Expr* th, Expr* el)
{
    return new ConditionalOperator(co, th, el, QualType());
}

ConditionalOperator* ConditionalOperator::NewObj(Expr* co, Expr* th, Expr* el, QualType qt)
{
    return new ConditionalOperator(co, th, el, qt);
}







