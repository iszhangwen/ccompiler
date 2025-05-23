#pragma once
#include "ast.h"

class Expr;

/*
(6.8) statement:
        labeled-statement
        compound-statement
        expression-statement
        selection-statement
        iteration-statement
        jump-statement
*/
class Stmt : public AstNode {
public:
    Stmt():AstNode(NodeKind::Expr){}
    virtual ~Stmt(){};
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    Stmt(NodeKind kd):AstNode(kd){}
};

//  identifier : statement
class LabelStmt final : public Stmt {
public:    
    static std::shared_ptr<LabelStmt> NewObj(SourceLocation loc, const std::string& name);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    LabelStmt(SourceLocation loc, const std::string& name): Stmt(), labelVal(name), loc_(loc){}
    SourceLocation loc_;
    std::string labelVal;
};

//  case constant-expression : statement
class CaseStmt final : public Stmt {
    public:    
    static std::shared_ptr<CaseStmt> NewObj(SourceLocation loc, std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmt);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    CaseStmt(SourceLocation loc, std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmt)
    : Stmt(), loc_(loc), expr_(expr), stmt_(stmt){}
    SourceLocation loc_;
    std::shared_ptr<Expr> expr_; // constant-expression
    std::shared_ptr<Stmt> stmt_; // statement
};

// default : statement
class DefaultStmt : public Stmt {
    public:    
    static std::shared_ptr<DefaultStmt> NewObj(SourceLocation loc, std::shared_ptr<Stmt> stmt);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    DefaultStmt(SourceLocation loc, std::shared_ptr<Stmt> stmt)
    : Stmt(), loc_(loc), stmt_(stmt){}
    SourceLocation loc_;
    std::shared_ptr<Stmt> stmt_; // statement
};

/*
(6.8.2) compound-statement:
            { block-item-listopt }
 (6.8.2) block-item-list:
            block-item
            block-item-list block-item
 (6.8.2) block-item:
            declaration
            statement
*/ 
class CompoundStmt final : public Stmt {
public:    
    static std::shared_ptr<CompoundStmt> NewObj(SourceLocation loc, std::vector<std::shared_ptr<Stmt>>& arrayStmt);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    CompoundStmt(SourceLocation loc,  std::vector<std::shared_ptr<Stmt>>& arrayStmt)
    : Stmt(), loc_(loc), arrayStmt_(arrayStmt){}
    SourceLocation loc_;
    std::vector<std::shared_ptr<Stmt>> arrayStmt_;
};

//  if ( expression ) statement
//  if ( expression ) statement else statement
class IfStmt : public Stmt {
public:    
    static std::shared_ptr<IfStmt> NewObj(SourceLocation loc, std::shared_ptr<Stmt> cond, std::shared_ptr<Stmt> then, std::shared_ptr<Stmt> els);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    IfStmt(SourceLocation loc,  std::shared_ptr<Expr> cond, std::shared_ptr<Stmt> then, std::shared_ptr<Stmt> els)
    : Stmt(), loc_(loc), cond_(cond), then_(then), else_(els){}
    SourceLocation loc_;
    std::shared_ptr<Expr> cond_;
    std::shared_ptr<Stmt> then_;
    std::shared_ptr<Stmt> else_;
};

//  switch ( expression ) statement
class SwitchStmt : public Stmt {
public:    
    static std::shared_ptr<SwitchStmt> NewObj(SourceLocation loc, std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmt);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    SwitchStmt(SourceLocation loc, std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmt)
    : Stmt(), loc_(loc), expr_(expr), stmt_(stmt){}
    SourceLocation loc_;
    std::shared_ptr<Expr> expr_; // expression
    std::shared_ptr<Stmt> stmt_; // statement
};

// while ( expression ) statement
class WhileStmt : public Stmt {
public:    
    static std::shared_ptr<WhileStmt> NewObj(SourceLocation loc, std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmt);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    WhileStmt(SourceLocation loc, std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmt)
    : Stmt(), loc_(loc), expr_(expr), stmt_(stmt){}
    SourceLocation loc_;
    std::shared_ptr<Expr> expr_; // expression
    std::shared_ptr<Stmt> stmt_; // statement
};

// do statement while ( expression );
class DoStmt : public Stmt {
public:    
    static std::shared_ptr<DoStmt> NewObj(SourceLocation loc, std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmt);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    DoStmt(SourceLocation loc, std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmt)
    : Stmt(), loc_(loc), expr_(expr), stmt_(stmt){}
    SourceLocation loc_;
    std::shared_ptr<Expr> expr_; // expression
    std::shared_ptr<Stmt> stmt_; // statement
};

//  for ( expressionopt ; expressionopt ; expressionopt ) statement
//  for ( declaration expressionopt ; expressionopt ) statement 
class ForStmt : public Stmt {
public:    
    static std::shared_ptr<ForStmt> NewObj(SourceLocation loc, std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmt);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    ForStmt(SourceLocation loc, std::shared_ptr<Stmt> init, std::shared_ptr<Stmt> cond, std::shared_ptr<Stmt> inc, std::shared_ptr<Stmt> body)
    : Stmt(), loc_(loc), init_(init), cond_(cond), inc_(inc), body_(body){}
    SourceLocation loc_;
    std::shared_ptr<Stmt> init_;  // init statement
    std::shared_ptr<Stmt> cond_;  // cond statement
    std::shared_ptr<Stmt> inc_;   // inc statement
    std::shared_ptr<Stmt> body_;  // body statement
};

// goto identifier ;
class GotoStmt final : public Stmt {
public:    
    static std::shared_ptr<GotoStmt> NewObj(SourceLocation loc, std::shared_ptr<Stmt> label);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    GotoStmt(SourceLocation loc, std::shared_ptr<Expr> expr, std::shared_ptr<Stmt> stmt)
    : Stmt(), loc_(loc), label_(stmt){}
    SourceLocation loc_;
    std::shared_ptr<Stmt> label_; // expression
};

// continue ;
class ContinueStmt final : public Stmt {
public:    
    static std::shared_ptr<ContinueStmt> NewObj(SourceLocation loc);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    ContinueStmt(SourceLocation loc)
    : Stmt(), loc_(loc){}
    SourceLocation loc_;
};

//  break ;
class BreakStmt final : public Stmt {
public:    
    static std::shared_ptr<BreakStmt> NewObj(SourceLocation loc);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    BreakStmt(SourceLocation loc)
    : Stmt(), loc_(loc){}
    SourceLocation loc_;
};

//  return expressionopt ;
class ReturnStmt final : public Stmt {
public:    
    static std::shared_ptr<ReturnStmt> NewObj(SourceLocation loc, std::shared_ptr<Expr> expr);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    ReturnStmt(SourceLocation loc, std::shared_ptr<Expr> expr)
    : Stmt(), loc_(loc), expr_(expr){}
    SourceLocation loc_;
    std::shared_ptr<Expr> expr_;
};

// ;
class EmptyStmt final : public Stmt {
public:    
    static std::shared_ptr<EmptyStmt> NewObj(SourceLocation loc);
    virtual void accept(std::shared_ptr<Vistor> vt);

private:
    EmptyStmt(SourceLocation loc)
    : Stmt(), loc_(loc){}
    SourceLocation loc_;
};

// 声明语句
class DeclStmt : Stmt {

};

// 表达式也是继承于语句
class Expr : public Stmt
{
public:
    virtual void accept(std::shared_ptr<Vistor> vt){}
    Expr(){}
private:

};