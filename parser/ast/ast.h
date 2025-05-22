#pragma once
#include <memory>

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

// 存储限定符
enum class StorageClass
{
    TYPEDEF = 0X1,
    EXTERN = 0X2,
    STATIC = 0X4,
    AUTO = 0X8,
    REGISTER = 0x10
};

// 类型说明符
enum class TypeSpec
{
    VOID,
    CHAR,
    SHORT,
    INT,
    LONG,
    FLOAT,
    DOUBLE,
    SIGNAED,
    UNSIGNED,
    _BOOL,
    _COMPLEX,
    // 自定义类型
    STRUCT_OR_UNION,
    ENUM,
    TYPEDEF_NAME
};

class Vistor
{

};

class AstNode {
public:
    AstNode(NodeKind nk): kind_(nk){}
    virtual ~AstNode() = default;
    virtual void accept(Vistor* vt) {}
    virtual NodeKind getKind() const {
        return kind_;
    }

private:
    NodeKind kind_;
};



