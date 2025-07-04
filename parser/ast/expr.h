#pragma once
#include "stmt.h"
class QualType;

class Expr : public Stmt
{
    QualType *qual_;
};

class DeclRefExpr : public Expr 
{

};

class PredefinedExpr : public Expr 
{

};

class IntegerLiteral : public Expr 
{

};

class CharacterLiteral : public Expr 
{

};

class FloatingLiteral : public Expr 
{

};

class ImaginaryLiteral : public Expr 
{

};

class StringLiteral : public Expr 
{

};

class ParenExpr : public Expr 
{

};

class UnaryOperator : public Expr 
{

};

class SizeOfAlignOfExpr : public Expr 
{

};

class ArraySubscriptExpr : public Expr 
{

};

class CallExpr : public Expr 
{

};

class MemberExpr : public Expr 
{

};

class CompoundLiteralExpr : public Expr 
{

};

class CastExpr : public Expr 
{

};

class ImplicitCastExpr : public CastExpr 
{

};

class ExplicitCastExpr : public CastExpr 
{

};

class CStyleCastExpr : public ExplicitCastExpr 
{

};

class BinaryOperator : public Expr 
{

};

class CompoundAssignOperator : public BinaryOperator 
{

};

class ConditionalOperator : public Expr 
{

};

class AddrLabelExpr : public Expr 
{

};

class InitListExpr : public Expr 
{

};

class DesignatedInitExpr : public Expr 
{

};

class ImplicitValueInitExpr : public Expr 
{

};

class ParenListExpr : public Expr 
{

};