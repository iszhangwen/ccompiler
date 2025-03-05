#pragma once

// 访问者接口
class Vistor {
public:
    virtual void visit(const class AstNode& node) {}
};

// ast基类
class AstNode {
public:
    virtual ~AstNode() = default;
    virtual void accept(Vistor* vt) const {}
};

// 表达式
class Expr : public AstNode {

};

class IdentifierExpr : public Expr {

};

class ConstantExpr : public Expr {

};

class StringLiteralExpr : public Expr {

};

