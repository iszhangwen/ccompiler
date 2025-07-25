#pragma once
#include "stmt.h"
#include "type.h"

class Expr : public Stmt
{
    QualType qual_;
protected:
    Expr(NodeKind nk, QualType qt)
    : Stmt(nk), qual_(qt){}

public:
    QualType getType() const {
        return qual_;
    }
    void setType(const QualType& qt) {
        qual_ = qt;
    }
};

class DeclRefExpr : public Expr 
{
    NamedDecl* decl_;
protected:
    DeclRefExpr(QualType qt, NamedDecl* dc)
    : Expr(NodeKind::NK_DeclRefExpr, qt), decl_(dc) {}
public:
    static DeclRefExpr* NewObj(QualType qt, NamedDecl* dc);
    NamedDecl* getNameDecl() const {
        return decl_;
    }
};

class IntegerLiteral : public Expr 
{
    std::string val_;
protected:
    IntegerLiteral(QualType qt, const std::string& val)
    : Expr(NodeKind::NK_IntegerLiteral, qt), val_(val){}
public:
    static IntegerLiteral* NewObj(Token*);
};

class CharacterLiteral : public Expr 
{
    std::string val_;
protected:
    CharacterLiteral(QualType qt, const std::string& val)
    : Expr(NodeKind::NK_CharacterLiteral, qt), val_(val) {}
public:
    static CharacterLiteral* NewObj(Token*);
};

class FloatingLiteral : public Expr 
{
    std::string val_;
protected:
    FloatingLiteral(QualType qt, const std::string& val)
    : Expr(NodeKind::NK_FloatingLiteral, qt), val_(val) {}
public:
    static FloatingLiteral* NewObj(Token*);
};

class StringLiteral : public Expr 
{
    std::string val_;
protected:
    StringLiteral(QualType qt, const std::string& val)
    : Expr(NodeKind::NK_StringLiteral, qt), val_(val) {}
public:
    static StringLiteral* NewObj(Token*);
};

class ParenExpr : public Expr 
{
    Stmt* val_;
protected:
    ParenExpr(Expr* val)
    : Expr(NodeKind::NK_ParenExpr, val->getType()), val_(val) {}
public:
    static ParenExpr* NewObj(Expr* val);
    Expr* getSubExpr() const {
        return dynamic_cast<Expr*>(val_);
    }
    void setSubExpr(Expr* val) {
        setType(val->getType());
        val_ = static_cast<Stmt*>(val);
    }
};

class UnaryOpExpr : public Expr 
{
public:
    enum OpCode {
        Post_Increment_, Post_Decrement_, // [C99 6.5.2.4] Postfix increment and decrement operators
        Pre_Increment_, Pre_Decrement_,   // [C99 6.5.3.1] Prefix increment and decrement operators.
        BitWise_AND_, Multiplication_,    // [C99 6.5.3.2] Address and indirection operators.
        Addition_, Subtraction_,      // [C99 6.5.3.3] Unary arithmetic operators.
        BitWise_NOT_, Logical_NOT_        // [C99 6.5.3.3] Unary arithmetic operators.
    };
private:
    Stmt* val_;
    OpCode op_;
protected:
    UnaryOpExpr(QualType qt, Expr* val, OpCode op)
    : Expr(NodeKind::NK_UnaryOperator, qt), val_(val), op_(op) {}
public:
    static UnaryOpExpr* NewObj(Expr* val, OpCode op);
    static UnaryOpExpr* NewObj(QualType qt, Expr* val, OpCode op);
    OpCode getOpCode() const {
        return op_;
    }
    void setOpCode(OpCode op) {
        op_ = op;
    }
    Expr* getSubExpr() const {
        return dynamic_cast<Expr*>(val_);
    }
    void setSubExpr(Expr* ex) {
        val_ = static_cast<Expr*>(ex);
    }
};

// 数组索引表达式
class ArraySubscriptExpr : public Expr 
{
    Stmt* base_;
    Stmt* index_;
protected:
    ArraySubscriptExpr(Expr* base, Expr* index)
    : Expr(NodeKind::NK_ArraySubscriptExpr, base->getType()), 
    base_(base), index_(index) {} 
public:
    static ArraySubscriptExpr* NewObj(Expr* base, Expr* index);
    Expr* getBaseExpr() const {
        return dynamic_cast<Expr*>(base_);
    }
    void setBaseExpr(Expr* ex) {
        setType(ex->getType());
        index_= static_cast<Stmt*>(ex);
    }
    Expr* getIndexExpr() const {
        return dynamic_cast<Expr*>(base_);
    }
    void setIndexExpr(Expr* ex) {
        index_ = static_cast<Stmt*>(ex);
    }
};

class CallExpr : public Expr 
{
    Stmt* fn_;
    std::vector<Stmt*> params_;
protected:
    CallExpr(Expr* fn, const std::vector<Expr*>& params)
    : Expr(NodeKind::NK_CallExpr, fn->getType()), fn_(fn){
        params_.clear();
        for (int i = 0; i < params_.size(); i++) {
            params_.emplace_back(static_cast<Expr*>(params_[i]));
        }
    }
public:
    static CallExpr* NewObj(Expr* fn, const std::vector<Expr*>& params);
    Expr* getCallee() {
        return static_cast<Expr*>(fn_);
    }
    void setCallee(Expr* fn) {
        setType(fn->getType());
        fn_ = dynamic_cast<Stmt*>(fn);
    }
    //FunctionDecl* getCalleeDecl();
    std::vector<Expr*> getParams() const {
        std::vector<Expr*> ret;
        for (int i = 0; i < params_.size(); i++) {
            ret.emplace_back(static_cast<Expr*>(params_[i]));
        }
        return ret;
    }
    void setParams(const std::vector<Stmt*>& params) {
        params_.clear();
        for (int i = 0; i < params.size(); i++) {
            params_.emplace_back(static_cast<Stmt*>(params[i]));
        }
    }
};

class MemberExpr : public Expr 
{
    Stmt* base_;
    NamedDecl* memberDecl_;
protected:
    bool isArrow_;
    MemberExpr(Expr* base, bool isArrow, NamedDecl* member, QualType qt)
    : Expr(NodeKind::NK_MemberExpr, qt), base_(base), memberDecl_(member), isArrow_(isArrow) {}
public:
    static MemberExpr* NewObj(Expr* base, bool isArrow, NamedDecl* member, QualType qt);
    Expr* getBase() {
        return static_cast<Expr*>(base_);
    }
    void setBase(Expr* ex) {
        base_ = dynamic_cast<Stmt*>(ex);
    }

    NamedDecl* getMemberDecl() {
        return memberDecl_;
    }
    void setMemberDecl(NamedDecl* member) {
        memberDecl_ = member;
    }
};

class CompoundLiteralExpr : public Expr 
{
    Stmt* init_;
protected:
    CompoundLiteralExpr(QualType qt, Expr* ex)
    : Expr(NodeKind::NK_CompoundLiteralExpr, qt), init_(ex) {}
public:
    static CompoundLiteralExpr* NewObj(QualType qt, Expr* ex);
    Expr *getInitExpr() { 
        return dynamic_cast<Expr*>(init_);
    }
    void setInitExpr(Expr *ex) { 
        init_ = static_cast<Stmt*>(ex);
    }
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
private:
    Stmt* val_;
    CastKind ck_;
protected:
    CastExpr(QualType qt, Expr* ex, CastKind ck)
    : Expr(NodeKind::NK_CastExpr, qt), ck_(ck) {}
public:
    static CastExpr* NewObj(QualType qt, Expr* ex, CastKind ck);
    Expr *getCastExpr() { 
        return dynamic_cast<Expr*>(val_);
    }
    void setCastExpr(Expr *ex) { 
        val_ = static_cast<Stmt*>(ex);
    }

    CastKind getCastKind() { 
        return ck_;
    }
    void setCastKind(CastKind ck) { 
        ck_ = ck;
    }
};

class BinaryOpExpr : public Expr 
{
public:
    enum OpCode {
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
private:
    Stmt* LExpr_;
    Stmt* RExpr_;
    OpCode op_;
protected:
    BinaryOpExpr(Expr* LE, Expr* RE, OpCode op, QualType qt)
    : Expr(NodeKind::NK_BinaryOperator, qt), LExpr_(LE), RExpr_(RE), op_(op) {}
public:
    static BinaryOpExpr* NewObj(Expr* LE, Expr* RE, OpCode op);
    static BinaryOpExpr* NewObj(Expr* LE, Expr* RE, OpCode op, QualType qt);
    OpCode getOpCode() const {
        return op_;
    }
    void setOpCode(OpCode op) {
        op_ = op;
    }
    Expr* getLExpr() {
        return static_cast<Expr*>(LExpr_);
    }
    void setLExpr(Expr* ex) {
        LExpr_ = dynamic_cast<Stmt*>(ex);
    }
    Expr* getRExpr() {
        return static_cast<Expr*>(RExpr_);
    }
    void setRExpr(Expr* ex) {
        RExpr_ = dynamic_cast<Stmt*>(ex);
    }
};

class ConditionalOperator : public Expr 
{
    Stmt* cond_;
    Stmt* then_;    
    Stmt* else_; 
protected:
    ConditionalOperator(Expr* co, Expr* th, Expr* el, QualType qt)
    : Expr(NodeKind::NK_ConditionalOperator, qt), cond_(co), then_(th), else_(el) {}
public:
    static ConditionalOperator* NewObj(Expr* co, Expr* th, Expr* el);
    static ConditionalOperator* NewObj(Expr* co, Expr* th, Expr* el, QualType qt);
    Expr* getCond() {
        return dynamic_cast<Expr*>(cond_);
    }
    void setCond(Expr* ex) {
        cond_ = static_cast<Stmt*>(ex);
    }
    Expr* getThen() {
        return dynamic_cast<Expr*>(then_);
    }
    void setThen(Expr* ex) {
        then_ = static_cast<Stmt*>(ex);
    }
    Expr* getElse() {
        return dynamic_cast<Expr*>(else_);
    }
    void setElse(Expr* ex) {
        else_ = static_cast<Stmt*>(ex);
    }
};

