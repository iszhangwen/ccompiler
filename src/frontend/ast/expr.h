#pragma once
#include "stmt.h"
#include <any>

class Expr : public Stmt
{
public:
    Expr(NodeKind nk, QualType qt)
    : Stmt(nk), m_qual(qt){}

    QualType getType() const {return m_qual;}
    void setType(const QualType& qt) {m_qual = qt;}
private:
    QualType m_qual;
};

class DeclRefExpr : public Expr 
{
public:
    DeclRefExpr()
    : Expr(NodeKind::NK_DeclRefExpr, QualType()), m_decl(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<NamedDecl> getDecl() const {return m_decl;}
    void setDecl(std::shared_ptr<NamedDecl> dc) {m_decl = dc;}
private:
    std::shared_ptr<NamedDecl> m_decl;
};

class IntegerLiteral : public Expr 
{
public:
    IntegerLiteral()
    : Expr(NodeKind::NK_IntegerLiteral, QualType()), m_val(0){}

    virtual void accept(ASTVisitor* vt) override;
    void setValue(std::any val) {m_val = val;}
    int64_t getSignedValue() {return std::any_cast<int64_t>(m_val);}
    uint64_t getUnsignedValue() {return std::any_cast<uint64_t>(m_val);}
private:
    std::any m_val;
};

class CharacterLiteral : public Expr 
{
public:
    CharacterLiteral()
    : Expr(NodeKind::NK_CharacterLiteral, QualType()), m_val('0') {}

    virtual void accept(ASTVisitor* vt) override;
    void setValue(std::any val) {m_val = val;}
    char getValue() {return std::any_cast<char>(m_val);}
private:
    std::any m_val;
};

class FloatingLiteral : public Expr 
{
public:
    FloatingLiteral()
    : Expr(NodeKind::NK_FloatingLiteral, QualType()), m_val(0.0f) {}

    virtual void accept(ASTVisitor* vt) override;
    void setValue(std::any val) {m_val = val;}
    float getFloatValue() {return std::any_cast<float>(m_val);}
    double getDoubleValue() {return std::any_cast<double>(m_val);}
    long double getLongDoubleValue() {return std::any_cast<long double>(m_val);}
private:
    std::any m_val;
};

class StringLiteral : public Expr 
{
public:
    StringLiteral()
    : Expr(NodeKind::NK_StringLiteral, QualType()), m_val("") {}

    virtual void accept(ASTVisitor* vt) override;
    void setValue(std::any val) {m_val = val;}
    std::string getValue() {return std::any_cast<std::string>(m_val);}
private:
    std::any m_val;
};

class ParenExpr : public Expr 
{
public:
    ParenExpr()
    : Expr(NodeKind::NK_ParenExpr, QualType()), m_expr(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getSubExpr() {return m_expr;}
    void setSubExpr(std::shared_ptr<Expr> val) {m_expr = val;}
private:
    std::shared_ptr<Expr> m_expr;
};

class UnaryOpExpr : public Expr 
{
public:
    enum OpCode {
        Unkonwn,
        Post_Increment_, Post_Decrement_, // [C99 6.5.2.4] Postfix increment and decrement operators
        Pre_Increment_, Pre_Decrement_,   // [C99 6.5.3.1] Prefix increment and decrement operators.
        BitWise_AND_, Multiplication_,    // [C99 6.5.3.2] Address and indirection operators.
        Addition_, Subtraction_,      // [C99 6.5.3.3] Unary arithmetic operators.
        BitWise_NOT_, Logical_NOT_        // [C99 6.5.3.3] Unary arithmetic operators.
    };

    UnaryOpExpr()
    : Expr(NodeKind::NK_UnaryOperator, QualType()), m_expr(nullptr), m_opCode(OpCode::Unkonwn) {}

    virtual void accept(ASTVisitor* vt) override;
    OpCode getOpCode() const {return m_opCode;}
    void setOpCode(OpCode op) {m_opCode = op;}
    std::shared_ptr<Expr> getSubExpr() {return m_expr;}
    void setSubExpr(std::shared_ptr<Expr> ex) {m_expr = ex;}

private:
    OpCode m_opCode;
    std::shared_ptr<Expr> m_expr;
};

// 数组索引表达式
class ArraySubscriptExpr : public Expr 
{
public:
    ArraySubscriptExpr()
    : Expr(NodeKind::NK_ArraySubscriptExpr, QualType()), m_base(nullptr), m_index(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getBaseExpr() {return m_base;}
    void setBaseExpr(std::shared_ptr<Expr> ex) {m_base = ex;}
    std::shared_ptr<Expr> getIndexExpr() {return m_index;}
    void setIndexExpr(std::shared_ptr<Expr> ex) {m_index = ex;}

private:
    std::shared_ptr<Expr> m_base;
    std::shared_ptr<Expr> m_index;
};

class CallExpr : public Expr 
{
public:
    using ParamExprGroup = std::vector<std::shared_ptr<Expr>>;
    CallExpr()
    : Expr(NodeKind::NK_CallExpr, QualType()), m_callee(nullptr){}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getCallee() {return m_callee;}
    void setCallee(std::shared_ptr<Expr> callee) {m_callee = callee;}
    ParamExprGroup getParams() {return m_paramExprs;}
    void setParams(const ParamExprGroup& params) {m_paramExprs = params;}

private:
    std::shared_ptr<Expr> m_callee;
    ParamExprGroup m_paramExprs;
};

class MemberExpr : public Expr 
{
public:
    MemberExpr()
    : Expr(NodeKind::NK_MemberExpr, QualType()), m_parent(nullptr), m_member(nullptr), m_isArrow(false) {}

    virtual void accept(ASTVisitor* vt) override;
    bool getIsArrow() {return m_isArrow;}
    void setIsArrow(bool flag) {m_isArrow = flag;}
    std::shared_ptr<DeclRefExpr> getParent() {return m_parent;}
    void setParent(std::shared_ptr<DeclRefExpr> ex) {m_parent = ex;}
    std::shared_ptr<DeclRefExpr> getMember() {return m_member;}
    void setMember(std::shared_ptr<DeclRefExpr> ex) {m_member = ex;}

private:
    bool m_isArrow;
    std::shared_ptr<DeclRefExpr> m_parent;
    std::shared_ptr<DeclRefExpr> m_member;
};

class CompoundLiteralExpr : public Expr 
{
public:
    CompoundLiteralExpr()
    : Expr(NodeKind::NK_CompoundLiteralExpr, QualType()), m_init(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getInitExpr() { return m_init;}
    void setInitExpr(std::shared_ptr<Expr> ex) { m_init = ex;}
private:
    std::shared_ptr<Expr> m_init;
};

class CastExpr : public Expr 
{
public:
    enum CastKind {
        CK_Unknown,               // 未知转换类型
        CK_ImplicitPromotion,     // 算术类型提升
        CK_ArrayToPointerDecay,   // 数组到指针衰减
        CK_AssignmentConversion,  // 赋值时的隐式转换
        CK_FunctionArgPromotion,  // 函数参数传递的类型提升
        CK_ExplicitCast,          // 显式C风格强制转换
        CK_VoidToPointer,         // void*到对象指针的隐式转换
        CK_VoidFromPointer,       // 对象指针到void*的隐式转换
        CK_restRICTCast,          // restrict指针转换
        CK_Coercion,              // 类型强制转换
        CK_LvalueCast,            // 左值转换
        CK_RvalueCast,            // 右值转换
    };

    CastExpr()
    : Expr(NodeKind::NK_CastExpr, QualType()), m_castKind(CK_Unknown) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getCastExpr() { return m_val;}
    void setCastExpr(std::shared_ptr<Expr> ex) { m_val = ex;}

    CastKind getCastKind() { return m_castKind;}
    void setCastKind(CastKind ck) { m_castKind = ck;}

private:
    CastKind m_castKind;
    std::shared_ptr<Expr> m_val;
};

class BinaryOpExpr : public Expr 
{
public:
    enum OpCode {
        Unknown,
        Multiplication_, Division_, Modulus_,    // [C99 6.5.5] Multiplicative operators.
        Addition_, Subtraction_,         // [C99 6.5.6] Additive operators.
        LShift_, RShift_,         // [C99 6.5.7] Bitwise shift operators.
        Less_, Greater_, Less_Equal_, Greater_Equal_,   // [C99 6.5.8] Relational operators.
        Equality_, Inequality_,           // [C99 6.5.9] Equality operators.
        BitWise_AND_,              // [C99 6.5.10] Bitwise AND operator.
        BitWise_XOR_,              // [C99 6.5.11] Bitwise XOR operator.
        BitWise_OR_,               // [C99 6.5.12] Bitwise OR operator.
        Logical_AND_,             // [C99 6.5.13] Logical AND operator.
        Logical_OR_,              // [C99 6.5.14] Logical OR operator.
        Assign_, Mult_Assign_,// [C99 6.5.16] Assignment operators.
        Div_Assign_, Mod_Assign_,
        Add_Assign_, Sub_Assign_,
        LShift_Assign_, RShift_Assign_,
        BitWise_AND_Assign_, BitWise_XOR_Assign_,
        BitWise_OR_Assign_,
        Comma             // [C99 6.5.17] Comma operator.
    };

    BinaryOpExpr()
    : Expr(NodeKind::NK_BinaryOperator, QualType()), m_lexpr(nullptr), m_rexpr(nullptr), m_opcode(Comma){}

    virtual void accept(ASTVisitor* vt) override;
    OpCode getOpCode() const {return m_opcode;}
    void setOpCode(OpCode op) {m_opcode = op;}
    std::shared_ptr<Expr> getLExpr() {return m_lexpr;}
    void setLExpr(std::shared_ptr<Expr> ex) {m_lexpr = ex;}
    std::shared_ptr<Expr> getRExpr() {return m_rexpr;}
    void setRExpr(std::shared_ptr<Expr> ex) {m_rexpr = ex;}

private:
    OpCode m_opcode;
    std::shared_ptr<Expr> m_lexpr;
    std::shared_ptr<Expr> m_rexpr;
};

class ConditionalExpr : public Expr 
{
public:
    ConditionalExpr()
    : Expr(NodeKind::NK_ConditionalOperator, QualType()), m_cond(nullptr), m_then(nullptr), m_else(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getCond() {return m_cond;}
    void setCond(std::shared_ptr<Expr> ex) {m_cond = ex;}
    std::shared_ptr<Expr> getThen() {return m_then;}
    void setThen(std::shared_ptr<Expr> ex) {m_then = ex;}
    std::shared_ptr<Expr> getElse() {return m_else;}
    void setElse(std::shared_ptr<Expr> ex) {m_else = ex;}

private:
    std::shared_ptr<Expr> m_cond;
    std::shared_ptr<Expr> m_then;
    std::shared_ptr<Expr> m_else;
};

