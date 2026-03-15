#pragma once
#include "ast.h"
#include "type.h"
#include "token.h"

class LabelStmt : public Stmt
{
public:
    LabelStmt()
    : Stmt(NodeKind::NK_LabelStmt), m_key(nullptr), m_val(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    NamedDecl* getLabel() {return m_key;}
    void setLabel(NamedDecl* key) {m_key = key;}

    Stmt* getStmt() {return m_val;}
    void setLabel(Stmt* val) {m_val = val;}

private:
    NamedDecl* m_key;
    Stmt* m_val;
};

class CaseStmt : public Stmt
{
public:
    CaseStmt()
    : Stmt(NodeKind::NK_CaseStmt), m_cond(nullptr), m_body(nullptr) {}
    virtual std::any accept(ASTVisitor* vt) override;
    // 获取和设置条件表达式和语句
    Expr* getCond() {return m_cond;}
    void setCond(Expr* cond) {m_cond = cond;}

    Stmt* getBody() {return m_body;}
    void setBody(Stmt* val) {m_body = val;}

private:
    Expr* m_cond;
    Stmt* m_body;
};

class DefaultStmt : public Stmt
{
public:
    DefaultStmt()
    : Stmt(NodeKind::NK_DefaultStmt), m_body(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    // 获取和设置条件表达式和语句
    Stmt* getBody() {return m_body;}
    void setBody(Stmt* val) {m_body = val;}

private:
    Stmt* m_body;
};

/*------------------------------compound-statement---------------------------------------------*/
class CompoundStmt : public Stmt
{
public:
    using StmtGroup = std::vector<Stmt*>;
    CompoundStmt()
    : Stmt(NodeKind::NK_CompoundStmt) {}

    CompoundStmt(const std::vector<Stmt*>& vals)
    : Stmt(NodeKind::NK_CompoundStmt){}

    virtual std::any accept(ASTVisitor* vt) override;
    StmtGroup getStmts() const {return m_bodys;}
    void setStmts(const StmtGroup& vals) {m_bodys = vals;}
    void addStmt(Stmt* st) {m_bodys.push_back(st);}
    void clearStmts() {m_bodys.clear();}
    size_t size() const {return m_bodys.size();}

private:
    StmtGroup m_bodys;
};

class DeclStmt : public Stmt
{
public:
    DeclStmt()
    : Stmt(NodeKind::NK_DeclStmt), m_decl(nullptr) {}
    DeclStmt(Decl* dc)
    : Stmt(NodeKind::NK_DeclStmt), m_decl(dc) {}

    virtual std::any accept(ASTVisitor* vt) override;
    Decl* getDecl() {return m_decl;}
    void setDecl(Decl* dc) {m_decl = dc;}

private:
    Decl* m_decl;
};

/*------------------------------表达式语句------------------------------------------------*/
class ExprStmt : public Stmt
{
public:
    ExprStmt()
    : Stmt(NodeKind::NK_ExprStmt), m_expr(nullptr){}
    ExprStmt(Expr* ex)
    : Stmt(NodeKind::NK_ExprStmt), m_expr(ex){}

    virtual std::any accept(ASTVisitor* vt) override;
    Expr* getExpr() {return m_expr;}
    void setExpr(Expr* ex) {m_expr = ex;}
private:
    Expr* m_expr;
};

class IfStmt : public Stmt
{
public:
    IfStmt()
    : Stmt(NodeKind::NK_IfStmt), m_cond(nullptr), m_then(nullptr), m_else(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    // 获取和设置条件表达式和语句
    Expr* getCond() {return m_cond;}
    void setCond(Expr* co) {m_cond = co;}
    Stmt* getThen() {return m_then;}
    void setThen(Stmt* th) {m_then = th;}
    Stmt* getElse() {return m_else;}
    void setElse(Stmt* el) {m_else = el;}

private:
    Expr* m_cond;
    Stmt* m_then;
    Stmt* m_else;
};

class SwitchStmt : public Stmt
{
public:
    SwitchStmt()
    : Stmt(NodeKind::NK_SwitchStmt), m_cond(nullptr), m_body(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    Expr* getCond() {return m_cond;}
    void setCond(Expr* co) {m_cond = co;}
    Stmt* getBody() {return m_body;}
    void setBody(Stmt* val) {m_body = val;}

private:
    Expr* m_cond;
    Stmt* m_body;
};

class WhileStmt : public Stmt
{
public:
    WhileStmt()
    : Stmt(NodeKind::NK_WhileStmt), m_cond(nullptr), m_body(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    Expr* getCond() {return m_cond;}
    void setCond(Expr* co) {m_cond = co;}
    Stmt* getBody() {return m_body;}
    void setBody(Stmt* val) {m_body = val;}
    // 获取条件表达式的类型
    QualType getCondType() const;
    // 获取语句的类型
    QualType getStmtType();

private:
    Expr* m_cond;
    Stmt* m_body;
};

class DoStmt : public Stmt
{
public:
    DoStmt()
    : Stmt(NodeKind::NK_DoStmt), m_cond(nullptr), m_body(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    Expr* getCond() {return m_cond;}
    void setCond(Expr* co) {m_cond = co;}
    Stmt* getBody() {return m_body;}
    void setBody(Stmt* val) {m_body = val;}

private:
    Expr* m_cond;
    Stmt* m_body;
};

class ForStmt : public Stmt
{
public:
    ForStmt()
    : Stmt(NodeKind::NK_ForStmt), m_init(nullptr), m_cond(nullptr), m_update(nullptr), m_body(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    // 设置默认值
    Expr* getInit() {return m_init;}
    void setInit(Expr* val) {m_init = val;}
    Expr* getCond() {return m_cond;}
    void setCond(Expr* val) {m_cond = val;}
    Expr* getUpdate() {return m_update;}
    void setUpdate(Expr* val) {m_update = val;}
    Stmt* getBody() {return m_body;}
    void setBody(Stmt* val) {m_body = val;}

private:
    Expr* m_init;
    Expr* m_cond;
    Expr* m_update;
    Stmt* m_body;
};

class GotoStmt : public Stmt
{
public:
    GotoStmt()
    : Stmt(NodeKind::NK_GotoStmt), m_label(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    Stmt* getLabel() {return m_label;}
    void setLabel(Stmt* lb) {m_label = lb;}

private:
    Stmt* m_label;
};

class ContinueStmt : public Stmt
{
public:
    ContinueStmt()
    : Stmt(NodeKind::NK_ContinueStmt), m_label(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    Stmt* getLabel() {return m_label;}
    void setLabel(Stmt* lb) {m_label = lb;}
private:
    Stmt* m_label;
};

class BreakStmt : public Stmt
{
public:
    BreakStmt()
    : Stmt(NodeKind::NK_BreakStmt), m_label(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    Stmt* getLabel() {return m_label;}
    void setLabel(Stmt* lb) {m_label = lb;}

private:
    Stmt* m_label;
};

class ReturnStmt : public Stmt
{
public:
    ReturnStmt()
    : Stmt(NodeKind::NK_ReturnStmt), m_retExpr(nullptr) {}

    virtual std::any accept(ASTVisitor* vt) override;
    Expr* getRetExpr() {return m_retExpr;}
    void setRetExpr(Expr* retVal) {m_retExpr = retVal;}

private:
    Expr* m_retExpr; // 返回值表达式
};
