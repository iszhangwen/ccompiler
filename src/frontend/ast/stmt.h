#pragma once
#include "ast.h"
#include "type.h"
#include "token.h"

class LabelStmt : public Stmt
{
public:
    LabelStmt()
    : Stmt(NodeKind::NK_LabelStmt), m_key(nullptr), m_val(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<NamedDecl> getLabel() {return m_key;}
    void setLabel(std::shared_ptr<NamedDecl> key) {m_key = key;}

    std::shared_ptr<Stmt> getStmt() {return m_val;}
    void setLabel(std::shared_ptr<Stmt> val) {m_val = val;}

private:
    std::shared_ptr<NamedDecl> m_key;
    std::shared_ptr<Stmt> m_val;
};

class CaseStmt : public Stmt
{
public:
    CaseStmt()
    : Stmt(NodeKind::NK_CaseStmt), m_cond(nullptr), m_body(nullptr) {}
    virtual void accept(ASTVisitor* vt) override;
    // 获取和设置条件表达式和语句
    std::shared_ptr<Expr> getCond() {return m_cond;}
    void setCond(std::shared_ptr<Expr> cond) {m_cond = cond;}

    std::shared_ptr<Stmt> getBody() {return m_body;}
    void setBody(std::shared_ptr<Stmt> val) {m_body = val;}

private:
    std::shared_ptr<Expr> m_cond;
    std::shared_ptr<Stmt> m_body;
};

class DefaultStmt : public Stmt
{
public:
    DefaultStmt()
    : Stmt(NodeKind::NK_DefaultStmt), m_body(val) {}

    virtual void accept(ASTVisitor* vt) override;
    // 获取和设置条件表达式和语句
    std::shared_ptr<Stmt> getBody() {return val_;}
    void setBody(std::shared_ptr<Stmt> val) {m_body = val;}

private:
    std::shared_ptr<Stmt> m_body;
};

/*------------------------------compound-statement---------------------------------------------*/
class CompoundStmt : public Stmt
{
public:
    using StmtGroup = std::vector<std::shared_ptr<Stmt>>;
    CompoundStmt()
    : Stmt(NodeKind::NK_CompoundStmt) {}

    CompoundStmt(const std::vector<Stmt*>& vals)
    : Stmt(NodeKind::NK_CompoundStmt), m_vals(vals) {}

    virtual void accept(ASTVisitor* vt) override;
    StmtGroup getStmts() const {return m_vals;}
    void setStmts(const StmtGroup& vals) {m_vals = vals;}
    void addStmt(std::shared_ptr<Stmt> st) {m_vals.push_back(st);}
    void clearStmts() {m_vals.clear();}
    size_t size() const {return m_vals.size();}

private:
    StmtGroup m_vals;
};

class DeclStmt : public Stmt
{
public:
    DeclStmt()
    : Stmt(NodeKind::NK_DeclStmt), m_decl(nullptr) {}
    DeclStmt(std::shared_ptr<Decl> dc)
    : Stmt(NodeKind::NK_DeclStmt), m_decl(dc) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Decl> getDecl() {return m_decl;}
    void setDecl(std::shared_ptr<Decl> dc) {m_decl = dc;}

private:
    std::shared_ptr<Decl> m_decl;
};

/*------------------------------表达式语句------------------------------------------------*/
class ExprStmt : public Stmt
{
public:
    ExprStmt()
    : Stmt(NodeKind::NK_ExprStmt), m_expr(nullptr){}
    ExprStmt(std::shared_ptr<Expr> ex)
    : Stmt(NodeKind::NK_ExprStmt), m_expr(ex){}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getExpr() {return m_expr;}
    void setExpr(std::shared_ptr<Expr> ex) {m_expr = ex;}
private:
    std::shared_ptr<Expr> m_expr;
};

class IfStmt : public Stmt
{
public:
    IfStmt()
    : Stmt(NodeKind::NK_IfStmt), m_cond(nullptr), m_then(nullptr), m_else(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    // 获取和设置条件表达式和语句
    std::shared_ptr<Expr> getCond() {return m_cond;}
    void setCond(std::shared_ptr<Expr> co) {m_cond = co;}
    std::shared_ptr<Stmt> getThen() {return m_then;}
    void setThen(std::shared_ptr<Stmt> th) {m_then = th;}
    std::shared_ptr<Stmt> getElse() {return m_else;}
    void setElse(std::shared_ptr<Stmt> el) {m_else = el;}

private:
    std::shared_ptr<Expr> m_cond;
    std::shared_ptr<Stmt> m_then;
    std::shared_ptr<Stmt> m_else;
};

class SwitchStmt : public Stmt
{
public:
    SwitchStmt()
    : Stmt(NodeKind::NK_SwitchStmt), m_cond(nullptr), m_body(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getCond() {return m_cond;}
    void setCond(std::shared_ptr<Expr> co) {m_cond = co;}
    std::shared_ptr<Stmt> getBody() {return m_body;}
    void setBody(std::shared_ptr<Stmt> val) {m_body = val;}

private:
    std::shared_ptr<Expr> m_cond;
    std::shared_ptr<Stmt> m_body;
};

class WhileStmt : public Stmt
{
public:
    WhileStmt()
    : Stmt(NodeKind::NK_WhileStmt), m_cond(nullptr), m_body(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getCond() {return m_cond;}
    void setCond(std::shared_ptr<Expr> co) {m_cond = co;}
    std::shared_ptr<Stmt> getBody() {return m_body;}
    void setBody(std::shared_ptr<Stmt> val) {m_body = val;}
    // 获取条件表达式的类型
    QualType getCondType() const;
    // 获取语句的类型
    QualType getStmtType();

private:
    std::shared_ptr<Expr> m_cond;
    std::shared_ptr<Stmt> m_body;
};

class DoStmt : public Stmt
{
public:
    DoStmt()
    : Stmt(NodeKind::NK_DoStmt), m_cond(nullptr), m_body(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getCond() {return m_cond;}
    void setCond(std::shared_ptr<Expr> co) {m_cond = co;}
    std::shared_ptr<Stmt> getBody() {return m_body;}
    void setBody(std::shared_ptr<Stmt> val) {m_body = val;}

private:
    std::shared_ptr<Expr> m_cond;
    std::shared_ptr<Stmt> m_body;
};

class ForStmt : public Stmt
{
public:
    ForStmt()
    : Stmt(NodeKind::NK_ForStmt), m_init(nullptr), m_cond(nullptr), m_update(nullptr), m_body(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    // 设置默认值
    std::shared_ptr<Expr> getInit() {return m_init;}
    void setInit(std::shared_ptr<Expr> val) {m_init = val;}
    std::shared_ptr<Expr> getCond() {return m_cond;}
    void setCond(std::shared_ptr<Expr> val) {m_cond = val;}
    std::shared_ptr<Expr> getUpdate() {return m_update;}
    void setUpdate(std::shared_ptr<Expr> val) {m_update = val;}
    std::shared_ptr<Stmt> getBody() {return m_body;}
    void setBody(std::shared_ptr<Stmt> val) {m_body = val;}

private:
    std::shared_ptr<Expr> m_init;
    std::shared_ptr<Expr> m_cond;
    std::shared_ptr<Expr> m_update;
    std::shared_ptr<Stmt> m_body;
};

class GotoStmt : public Stmt
{
public:
    GotoStmt()
    : Stmt(NodeKind::NK_GotoStmt), m_label(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Stmt> getLabel() {return m_label;}
    void setLabel(std::shared_ptr<Stmt> lb) {m_label = lb;}

private:
    std::shared_ptr<Stmt> m_label;
};

class ContinueStmt : public Stmt
{
public:
    ContinueStmt()
    : Stmt(NodeKind::NK_ContinueStmt), m_label(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Stmt> getLabel() {return m_label;}
    void setLabel(std::shared_ptr<Stmt> lb) {m_label = lb;}
private:
    std::shared_ptr<Stmt> m_label;
};

class BreakStmt : public Stmt
{
public:
    BreakStmt()
    : Stmt(NodeKind::NK_BreakStmt), m_label(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Stmt> getLabel() {return m_label;}
    void setLabel(std::shared_ptr<Stmt> lb) {m_label = lb;}

private:
    std::shared_ptr<Stmt> m_label;
};

class ReturnStmt : public Stmt
{
public:
    ReturnStmt()
    : Stmt(NodeKind::NK_ReturnStmt), m_retExpr(nullptr) {}

    virtual void accept(ASTVisitor* vt) override;
    std::shared_ptr<Expr> getRetExpr() {return m_retExpr;}
    void setRetExpr(std::shared_ptr<Expr> retVal) {m_retExpr = retVal;}

private:
    std::shared_ptr<Expr> m_retExpr; // 返回值表达式
};
