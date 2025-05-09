#pragma once
#include <source.h>
#include <memory>
#include "token.h"
#include "scope.h"

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
    // 声明
    LabelDecl,


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

// 链接属性
enum class Linkage
{
    EXTERNAL,
    INTERNAL,
    NONE
};

// 存储说明符
enum class storageSpec
{
    TYPEDEF,
    EXTERN,
    STATIC,
    AUTO,
    REGISTER
};

class AstNode {
public:
    AstNode(NodeKind nk): kind_(nk){}
    virtual ~AstNode() = default;
    virtual void accept(Vistor* vt) = 0;
    virtual NodeKind getKind() const {
        return kind_;
    }

private:
    NodeKind kind_;
};



