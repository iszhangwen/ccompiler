#pragma once
#include <source.h>
#include <memory>

class Vistor;

// AST上下文
class AstContext {
    
};

// 抽象语法树node类型
enum class NodeKind {
    // 额外声明
    TranslationUnitDecl,
    FunctionDecl,

    // 声明: 声明由声明说明符，初始化声明符列表组成
    Decl,
    // 声明说明符
    DeclSpec, 
    TypenameSpec,
    Object,


    // 表达式
    Expr,

    // 语句
    ExprStmt,
    LabelStmt,
    CaseStmt,
    DefaultStmt,
    IfStmt,
    SwitchStmt,
    WhileStmt,
    DoStmt,
    ForStmt,
    GotoStmt,
    ContinueStmt,
    BreakStmt,
    ReturnStmt,
    CompoundStmt
};

class AstNode {
public:
    virtual ~AstNode() = default;
    virtual void accept(Vistor* vt) = 0;

private:
    NodeKind kind_;
};



