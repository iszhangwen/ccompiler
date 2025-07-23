#pragma once
#include <memory>
#include <vector>

class TranslationUnitDecl;
class Type;
class Vistor;

class AstNode {
public:
    // 抽象语法树node类型
    enum NodeKind {
        // 翻译单元
        NK_TranslationUnitDecl,
        // 具名基类
        NK_NamedDecl,
        NK_LabelDecl,
        // 含值类型
        NK_ValueDecl,
        // 含声明说明符
        NK_DeclaratorDecl,
        // 变量
        NK_VarDecl,
        // 参数变量
        NK_ParmVarDecl,
        // 函数
        NK_FunctionDecl,
        // 类型别名
        NK_TypedefDecl,
        // 结构体/联合体字段声明
        NK_FieldDecl,
        NK_EnumConstantDecl,
        // 标签声明(结构体/联合体/枚举)
        NK_TagDecl,
        NK_EnumDecl,
        NK_RecordDecl,

        /* 表达式*/
        // 基本表达式
        NK_DeclRefExpr,
        NK_IntegerLiteral,
        NK_CharacterLiteral,
        NK_FloatingLiteral,
        NK_StringLiteral,
        NK_ParenExpr,
        // 后缀表达式
        NK_ArraySubscriptExpr,
        NK_CallExpr,
        NK_MemberExpr,
        NK_CompoundLiteralExpr,
        // 一元表达式
        NK_UnaryOperator,
        // 类型转换
        NK_CastExpr,
        // 二元表达式
        NK_BinaryOperator,
        // 条件表达式
        NK_ConditionalOperator,

        /*语句 NODE类型*/
        // 标签语句
        NK_LabelStmt,
        NK_CaseStmt,
        NK_DefaultStmt,
        // 复合语句
        NK_CompoundStmt,
        NK_DeclStmt,
        // 表达式语句
        NK_ExprStmt,
        // 控制流-选择语句
        NK_IfStmt,
        NK_SwitchStmt,
        // 控制流-迭代语句
        NK_WhileStmt,
        NK_DoStmt,
        NK_ForStmt,
        // 控制流-跳转语句
        NK_GotoStmt,
        NK_ContinueStmt,
        NK_BreakStmt,
        NK_ReturnStmt
    };

    AstNode(NodeKind nk): kind_(nk){}
    virtual ~AstNode() = default;
    virtual void accept(Vistor* vt) {}
    virtual NodeKind getKind() const {
        return kind_;
    }

private:
    NodeKind kind_;
};

// 声明说明符
enum StorageClass 
{
    TYPEDEF =   (1 << 1),
    EXTERN =    (1 << 2),
    STATIC =    (1 << 3),
    AUTO =      (1 << 4),
    REGISTER =  (1 << 5),
    SC_MASK = TYPEDEF | EXTERN | STATIC | AUTO | REGISTER
};
enum TypeSpecifier 
{
    NONE = 0,
    VOID =     (1 << 1),
    CHAR =     (1 << 2),
    INT =      (1 << 3),
    FLOAT =    (1 << 4),
    DOUBLE =   (1 << 5),
    SHORT =    (1 << 6),
    LONG =     (1 << 7),
    LONGLONG = (1 << 8),
    SIGNED =   (1 << 9),
    UNSIGNED = (1 << 10),
    _BOOL =    (1 << 11),
    _COMPLEX = (1 << 12),
    STRUCT =   (1 << 13),
    UNION =    (1 << 14),
    ENUM =     (1 << 15),
    LONGDOUBLE = (1 << 16)
};
enum TypeQualifier 
{
    CONST =    (1 << 1),
    RESTRICT = (1 << 2),
    VOLATILE = (1 << 3),
    TQ_MASK = CONST | RESTRICT | VOLATILE
};
enum FuncSpecifier 
{
    INLINE = (1 << 1),
    FS_MASK = INLINE
};

class Decl;
class Declarator;
using DeclGroup = std::vector<Decl*>;
using DeclaratorGroup = std::vector<Declarator>;

