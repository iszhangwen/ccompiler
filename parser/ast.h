#pragma once

// AST上下文
class AstContext {
    
};

// 访问者接口
class Vistor {
public:
    virtual void visit(const class AstNode& node) = 0;
};

// ast基类
class AstNode {
public:
    virtual ~AstNode() = default;
    virtual void accept(Vistor* vt) const {}
};

// 语句
using Stmt = AstNode*;
using LableStmt = AstNode*;
using CaseStmt = AstNode*;
using DefaultStmt = AstNode*;
using CompoundStmt = AstNode*;
using IfStmt = AstNode*;
using SwitchStmt = AstNode*;
using WhileStmt = AstNode*;
using DoStmt = AstNode*;
using ForStmt = AstNode*;
using GotoStmt = AstNode*;
using ContinueStmt = AstNode*;
using BreakStmt = AstNode*;
using ReturnStmt = AstNode*;

// 表达式
using PrimaryExpr = AstNode*;
using PostfixExpr = AstNode*;
using ArgumentExprList = AstNode*;
using UnaryExpr = AstNode*;
using CastExpr = AstNode*;
using MultiplicativeExpr = AstNode*;
using AdditiveExpr = AstNode*;
using ShiftExpr = AstNode*;
using RelationalExpr = AstNode*;
using EqualityExpr = AstNode*;
using ANDExpr = AstNode*;
using ExclusiveORExpr = AstNode*;
using LogicalANDExpr = AstNode*;
using LogicalORExpr = AstNode*;
using ConditionalExpr = AstNode*;
using AssignmentExpr = AstNode*;
using Expr = AstNode*;
using ConstantExpr = AstNode*;

// 声明
using Decl = AstNode*;


// external definitions

