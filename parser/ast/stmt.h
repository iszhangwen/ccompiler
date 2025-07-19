#pragma once
#include "ast.h"
#include <token.h>

/* (6.8) statement:
 labeled-statement
 compound-statement
 expression-statement
 selection-statement
 iteration-statement
 jump-statement
 按照C99文法定义，按照以上语句类型划分AST节点类型
*/

class Expr;
class Decl;
class NamedDecl;

/*------------------------------基类--------------------------------------------------*/
class Stmt : public AstNode {
public:
    Stmt(NodeKind kd)
    : AstNode(kd){}
    virtual ~Stmt(){};
};

/*------------------------------labeled-statement---------------------------------------------*/
class LabelStmt : public Stmt 
{
    NamedDecl* key_;
    Stmt* val_;
public:
    LabelStmt(NamedDecl* key, Stmt* val)
    : Stmt(NodeKind::NK_LabelStmt), key_(key), val_(val) {}

    NamedDecl* getLabel() {
        return key_;
    }
    void setLabel(NamedDecl* key) {
        key_ = key;
    }

    Stmt* getStmt() {
        return val_;
    }
    void setLabel(Stmt* val) {
        val_ = val;
    }
};

class CaseStmt : public Stmt 
{
    Expr* cond_;
    Stmt* val_;
public:
    CaseStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_CaseStmt), cond_(cond), val_(val) {}

    Expr* getCond() {
        return cond_;
    }
    void setCond(Expr* cond) {
        cond_ = cond;
    }

    Stmt* getStmt() {
        return val_;
    }
    void setLabel(Stmt* val) {
        val_ = val;
    }
};

class DefaultStmt : public Stmt 
{
    Stmt* val_;
public:
    DefaultStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_DefaultStmt), val_(val) {}

    Stmt* getStmt() {
        return val_;
    }
    void setLabel(Stmt* val) {
        val_ = val;
    }
};

/*------------------------------compound-statement---------------------------------------------*/
class CompoundStmt : public Stmt
{
    std::vector<Stmt*> vals_;
public:
    CompoundStmt()
    : Stmt(NodeKind::NK_CompoundStmt) {}

    CompoundStmt(const std::vector<Stmt*>& vals)
    : Stmt(NodeKind::NK_CompoundStmt), vals_(vals) {}
};

class DeclStmt : public Stmt {
    Decl* dc_;
public:
    DeclStmt(Decl* dc)
    : Stmt(NodeKind::NK_DeclStmt), dc_(dc) {}

    Decl* getDecl() {
        return dc_;
    }
    void setDecl(Decl* dc) {
        dc_ = dc;
    }
};

/*------------------------------表达式语句------------------------------------------------*/
class ExprStmt : public Stmt {
    Expr* ex_;
public:
    ExprStmt(Expr* ex)
    : Stmt(NodeKind::NK_ExprStmt), ex_(ex){}

    Expr* getExpr() {
        return ex_;
    }
    void setExpr(Expr* ex) {
        ex_ = ex;
    }
};

/*------------------------------控制流-条件语句---------------------------------------------*/
class IFStmt : public Stmt 
{
    Expr* cond_;
    Stmt* then_;
    Stmt* else_;
public:
    IFStmt(Expr* cond, Stmt* th, Stmt* el = nullptr)
    : Stmt(NodeKind::NK_IfStmt), cond_(cond), then_(th), else_(el) {}

    Expr* getCond() {
        return cond_;
    }
    void setCond(Expr* co) {
        cond_ = co;
    }
    Stmt* getThen() {
        return then_;
    }
    void setThen(Stmt* th) {
        then_ = th;
    }
    Stmt* getElse() {
        return else_;
    }
    void setElse(Stmt* el) {
        else_ = el;
    }
};

class SwitchStmt : public Stmt 
{
    Expr* cond_;
    Stmt* val_;
public:
    SwitchStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_SwitchStmt), cond_(cond), val_(val) {}

     Expr* getCond() {
        return cond_;
    }
    void setCond(Expr* co) {
        cond_ = co;
    }
    Stmt* getStmt() {
        return val_;
    }
    void setStmt(Stmt* val) {
        val_ = val;
    }
};


/*------------------------------控制流-迭代语句---------------------------------------------*/
class WhileStmt : public Stmt 
{
    Expr* cond_;
    Stmt* val_;
public:
    WhileStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_WhileStmt), cond_(cond), val_(val) {}

     Expr* getCond() {
        return cond_;
    }
    void setCond(Expr* co) {
        cond_ = co;
    }
    Stmt* getStmt() {
        return val_;
    }
    void setStmt(Stmt* val) {
        val_ = val;
    }
};

class DoStmt : public Stmt 
{
    Expr* cond_;
    Stmt* val_;
public:
    DoStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_DoStmt), cond_(cond), val_(val) {}

     Expr* getCond() {
        return cond_;
    }
    void setCond(Expr* co) {
        cond_ = co;
    }
    Stmt* getStmt() {
        return val_;
    }
    void setStmt(Stmt* val) {
        val_ = val;
    }
};

class ForStmt : public Stmt 
{
    Expr* init_;
    Expr* cond_;
    Expr* update_;
    Stmt* val_;
public:
    ForStmt(Expr* init, Expr* cond, Expr* update, Stmt* val)
    : Stmt(NodeKind::NK_ForStmt), init_(init), cond_(cond), update_(update), val_(val) {}
};


/*------------------------------控制流-跳转语句---------------------------------------------*/
class GotoStmt : public Stmt 
{
    Stmt* label_;
public:
    GotoStmt(Stmt* label)
    : Stmt(NodeKind::NK_GotoStmt), label_(label) {}

    Stmt* getLabel() {
        return label_;
    }
    void setLabel(Stmt* lb) {
        label_ = lb;
    }
};

class ContinueStmt : public Stmt 
{
    Stmt* label_;
public:
    ContinueStmt(Stmt* label)
    : Stmt(NodeKind::NK_ContinueStmt), label_(label) {}
};

class BreakStmt : public Stmt 
{
    Stmt* label_;
public:
    BreakStmt(Stmt* label)
    : Stmt(NodeKind::NK_BreakStmt), label_(label) {}
};

class ReturnStmt : public Stmt 
{
    Stmt* label_;
public:
    ReturnStmt(Stmt* label)
    : Stmt(NodeKind::NK_ReturnStmt), label_(label) {}
};
