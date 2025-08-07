#pragma once
#include "ast.h"
#include "type.h"
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
private:
    NamedDecl* key_;
    Stmt* val_;
public:
    LabelStmt(NamedDecl* key, Stmt* val)
    : Stmt(NodeKind::NK_LabelStmt), key_(key), val_(val) {}

    virtual void accept(ASTVisitor* vt) override;
    NamedDecl* getLabel() {return key_;}
    void setLabel(NamedDecl* key) {key_ = key;}

    Stmt* getStmt() {return val_;}
    void setLabel(Stmt* val) {val_ = val;}
};

class CaseStmt : public Stmt 
{
private:
    Expr* cond_;
    Stmt* val_;
public:
    CaseStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_CaseStmt), cond_(cond), val_(val) {}
    virtual void accept(ASTVisitor* vt) override;
    // 获取和设置条件表达式和语句
    Expr* getCond() {return cond_;}
    void setCond(Expr* cond) {cond_ = cond;}

    Stmt* getStmt() {return val_;}
    void setLabel(Stmt* val) {val_ = val;}
};

class DefaultStmt : public Stmt 
{
private:
    Stmt* val_;
public:
    DefaultStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_DefaultStmt), val_(val) {}

    virtual void accept(ASTVisitor* vt) override;
    // 获取和设置条件表达式和语句
    Expr* getCond() {return nullptr; }
    Stmt* getStmt() {return val_;}
    void setLabel(Stmt* val) {val_ = val;}
};

/*------------------------------compound-statement---------------------------------------------*/
class CompoundStmt : public Stmt
{
private:
    std::vector<Stmt*> vals_;

public:
    CompoundStmt()
    : Stmt(NodeKind::NK_CompoundStmt) {}
    CompoundStmt(const std::vector<Stmt*>& vals)
    : Stmt(NodeKind::NK_CompoundStmt), vals_(vals) {}
    virtual void accept(ASTVisitor* vt) override;

    std::vector<Stmt*> getStmts() const {return vals_;}
    void setStmts(const std::vector<Stmt*>& vals) {vals_ = vals;}
    void addStmt(Stmt* st) {vals_.push_back(st);}
    void clearStmts() {vals_.clear();}
    size_t size() const {return vals_.size();}
};

class DeclStmt : public Stmt 
{
private:
    Decl* dc_;
public:
    DeclStmt(Decl* dc)
    : Stmt(NodeKind::NK_DeclStmt), dc_(dc) {}
    virtual void accept(ASTVisitor* vt) override;

    Decl* getDecl() {return dc_;}
    void setDecl(Decl* dc) {dc_ = dc;} 
};

/*------------------------------表达式语句------------------------------------------------*/
class ExprStmt : public Stmt 
{
private:
    Expr* ex_;
public:
    ExprStmt(Expr* ex)
    : Stmt(NodeKind::NK_ExprStmt), ex_(ex){}
    virtual void accept(ASTVisitor* vt) override;

    Expr* getExpr() {return ex_;}
    void setExpr(Expr* ex) {ex_ = ex;}
};

/*------------------------------控制流-条件语句---------------------------------------------*/
class IfStmt : public Stmt 
{
    Expr* cond_;
    Stmt* then_;
    Stmt* else_;
protected:
    IfStmt(Expr* cond, Stmt* th, Stmt* el = nullptr)
    : Stmt(NodeKind::NK_IfStmt), cond_(cond), then_(th), else_(el) {}
public:
    static IfStmt* NewObj(Expr* cond, Stmt* th, Stmt* el = nullptr);
    virtual void accept(ASTVisitor* vt) override;
    // 获取和设置条件表达式和语句
    Expr* getCond() {return cond_;}
    void setCond(Expr* co) {cond_ = co;}

    Stmt* getThen() {return then_;}
    void setThen(Stmt* th) {then_ = th;}

    Stmt* getElse() {return else_;}
    void setElse(Stmt* el) {else_ = el;}
};

class SwitchStmt : public Stmt 
{
    Expr* cond_;
    Stmt* val_;
public:
    SwitchStmt(Expr* cond, Stmt* val)
    : Stmt(NodeKind::NK_SwitchStmt), cond_(cond), val_(val) {}
    virtual void accept(ASTVisitor* vt) override;
    Expr* getCond() {return cond_;}
    void setCond(Expr* co) {cond_ = co;}
    Stmt* getStmt() {return val_;}
    void setStmt(Stmt* val) {val_ = val;}
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
    virtual void accept(ASTVisitor* vt) override;
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
    // 获取条件表达式的类型
    QualType getCondType() const;
    // 获取语句的类型
    QualType getStmtType();
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
    virtual void accept(ASTVisitor* vt) override;
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
    virtual void accept(ASTVisitor* vt) override;
};


/*------------------------------控制流-跳转语句---------------------------------------------*/
class GotoStmt : public Stmt 
{
private:
    Stmt* label_;
public:
    GotoStmt(Stmt* label)
    : Stmt(NodeKind::NK_GotoStmt), label_(label) {}

    virtual void accept(ASTVisitor* vt) override;
    Stmt* getLabel() {return label_;}
    void setLabel(Stmt* lb) {label_ = lb;}
};

class ContinueStmt : public Stmt 
{
    Stmt* label_;
protected:
    ContinueStmt(Stmt* label)
    : Stmt(NodeKind::NK_ContinueStmt), label_(label) {}
public:
    static ContinueStmt* NewObj(Stmt* label);
    virtual void accept(ASTVisitor* vt) override;
    Stmt* getLabel() {
        return label_;
    }
    void setLabel(Stmt* lb) {
        label_ = lb;
    }
};

class BreakStmt : public Stmt 
{
    Stmt* label_;
protected:
    BreakStmt(Stmt* label)
    : Stmt(NodeKind::NK_BreakStmt), label_(label) {}
public:
    static BreakStmt* NewObj(Stmt* label);
    virtual void accept(ASTVisitor* vt) override;
    Stmt* getLabel() {
        return label_;
    }
    void setLabel(Stmt* lb) {
        label_ = lb;
    }
};

class ReturnStmt : public Stmt 
{
private:
    ExprStmt* retVal_; // 返回值表达式
public:
    ReturnStmt(ExprStmt* retVal)
    : Stmt(NodeKind::NK_ReturnStmt), retVal_(retVal) {}
    virtual void accept(ASTVisitor* vt) override;

    ExprStmt* getReturnValue() {return retVal_;}
    void setReturnValue(ExprStmt* retVal) {retVal_ = retVal;}
    // 获取返回值表达式的类型
    QualType getReturnType() const;
};
