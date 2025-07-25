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
protected:
    LabelStmt(NamedDecl* key, Stmt* val)
    : Stmt(NodeKind::NK_LabelStmt), key_(key), val_(val) {}
public:
    static LabelStmt* NewObj(NamedDecl* key, Stmt* val);

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
protected:
    CaseStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_CaseStmt), cond_(cond), val_(val) {}
public:
    static CaseStmt* NewObj(Expr* cond, Stmt* val);
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
protected:
    DefaultStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_DefaultStmt), val_(val) {}
public:
    static DefaultStmt* NewObj(Expr* cond, Stmt* val);
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
protected:
    CompoundStmt()
    : Stmt(NodeKind::NK_CompoundStmt) {}

    CompoundStmt(const std::vector<Stmt*>& vals)
    : Stmt(NodeKind::NK_CompoundStmt), vals_(vals) {}
public:
    static CompoundStmt* NewObj(const std::vector<Stmt*>& vals);
};

class DeclStmt : public Stmt {
    Decl* dc_;
protected:
    DeclStmt(Decl* dc)
    : Stmt(NodeKind::NK_DeclStmt), dc_(dc) {}
public:
    static DeclStmt* NewObj(Decl* dc);
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
protected:
    ExprStmt(Expr* ex)
    : Stmt(NodeKind::NK_ExprStmt), ex_(ex){}
public:
    static ExprStmt* NewObj(Expr* ex);
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
protected:
    IFStmt(Expr* cond, Stmt* th, Stmt* el = nullptr)
    : Stmt(NodeKind::NK_IfStmt), cond_(cond), then_(th), else_(el) {}
public:
    static IFStmt* NewObj(Expr* cond, Stmt* th, Stmt* el = nullptr);
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
protected:
    SwitchStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_SwitchStmt), cond_(cond), val_(val) {}
public:
    static SwitchStmt* NewObj(Expr* cond, Stmt* val);
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
protected:
    WhileStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_WhileStmt), cond_(cond), val_(val) {}
public:
    static WhileStmt* NewObj(Expr* cond, Stmt* val);
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
protected:
    DoStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_DoStmt), cond_(cond), val_(val) {}
public:
    static DoStmt* NewObj(Expr* cond, Stmt* val);
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
protected:
    ForStmt(Expr* init, Expr* cond, Expr* update, Stmt* val)
    : Stmt(NodeKind::NK_ForStmt), init_(init), cond_(cond), update_(update), val_(val) {}
public:
    static ForStmt* NewObj(Expr* init, Expr* cond, Expr* update, Stmt* val);
};


/*------------------------------控制流-跳转语句---------------------------------------------*/
class GotoStmt : public Stmt 
{
    Stmt* label_;
protected:
    GotoStmt(Stmt* label)
    : Stmt(NodeKind::NK_GotoStmt), label_(label) {}
public:
    static GotoStmt* NewObj(Stmt* label);
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
protected:
    ContinueStmt(Stmt* label)
    : Stmt(NodeKind::NK_ContinueStmt), label_(label) {}
public:
    static ContinueStmt* NewObj(Stmt* label);
};

class BreakStmt : public Stmt 
{
    Stmt* label_;
protected:
    BreakStmt(Stmt* label)
    : Stmt(NodeKind::NK_BreakStmt), label_(label) {}
public:
    static BreakStmt* NewObj(Stmt* label);
};

class ReturnStmt : public Stmt 
{
    Stmt* label_;
protected:
    ReturnStmt(Stmt* label)
    : Stmt(NodeKind::NK_ReturnStmt), label_(label) {}
public:
    static ReturnStmt* NewObj(Stmt* label);
};
