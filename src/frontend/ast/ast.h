#pragma once
#include <memory>
#include <vector>
#include <any>

#include "arena.h"

// AST节点前置声明
class AstNode;
using Stmt = AstNode;
using Decl = AstNode;
class Expr;
class Type;
class TranslationUnitDecl;
class LabelDecl;
class NamedDecl;
class ValueDecl;
class DeclaratorDecl;
class VarDecl;
class ParmVarDecl;
class FunctionDecl;
class FieldDecl;
class EnumConstantDecl;
class EnumDecl;
class TagDecl;
class RecordDecl;
class TypedefDecl;
class LabelStmt;
class CaseStmt;
class DefaultStmt;
class CompoundStmt;
class DeclStmt;
class ExprStmt;
class IfStmt;
class SwitchStmt;
class WhileStmt;
class DoStmt;
class ForStmt;
class GotoStmt;
class ContinueStmt;
class BreakStmt;
class ReturnStmt;
class IntegerLiteral;
class CharacterLiteral;
class FloatingLiteral;
class StringLiteral;
class DeclRefExpr;
class ParenExpr;
class BinaryOpExpr;
class ConditionalExpr;
class CompoundLiteralExpr;
class CastExpr;
class ArraySubscriptExpr;
class CallExpr;
class MemberExpr;
class UnaryOpExpr;
class SymbolTableContext;

// ast ir上下文
struct AstCtx {
    TranslationUnitDecl* ir;
    SymbolTableContext* symbol;
    AstCtx(): ir(nullptr), symbol(nullptr){}
    AstCtx(TranslationUnitDecl* i, SymbolTableContext* s): ir(i), symbol(s){}
    AstCtx(const AstCtx& obj): AstCtx(obj.ir, obj.symbol){}
};

// 通用访问接口
class ASTVisitor 
{
public:
    virtual ~ASTVisitor() = default;
    // 访问翻译单元节点
    virtual std::any visit(TranslationUnitDecl*) = 0;
    // 访问声明节点
    virtual std::any visit(LabelDecl*) = 0;
    virtual std::any visit(ValueDecl*) = 0;
    virtual std::any visit(DeclaratorDecl*) = 0;
    virtual std::any visit(VarDecl*) = 0;
    virtual std::any visit(ParmVarDecl*) = 0;
    virtual std::any visit(FunctionDecl*) = 0;
    virtual std::any visit(FieldDecl*) = 0;
    virtual std::any visit(EnumConstantDecl*) = 0;
    virtual std::any visit(TypedefDecl*) = 0;
    virtual std::any visit(EnumDecl*) = 0;
    virtual std::any visit(RecordDecl*) = 0;
    // 访问语句节点
    virtual std::any visit(LabelStmt*) = 0;
    virtual std::any visit(CaseStmt*) = 0;
    virtual std::any visit(DefaultStmt*) = 0;
    virtual std::any visit(CompoundStmt*) = 0;
    virtual std::any visit(DeclStmt*) = 0;
    virtual std::any visit(ExprStmt*) = 0;
    virtual std::any visit(IfStmt*) = 0;
    virtual std::any visit(SwitchStmt*) = 0;
    virtual std::any visit(WhileStmt*) = 0;
    virtual std::any visit(DoStmt*) = 0;
    virtual std::any visit(ForStmt*) = 0;
    virtual std::any visit(GotoStmt*) = 0;
    virtual std::any visit(ContinueStmt*) = 0;
    virtual std::any visit(BreakStmt*) = 0;
    virtual std::any visit(ReturnStmt*) = 0;
    // 访问表达式节点
    virtual std::any visit(IntegerLiteral*) = 0;
    virtual std::any visit(FloatingLiteral*) = 0;
    virtual std::any visit(CharacterLiteral*) = 0;
    virtual std::any visit(StringLiteral*) = 0;
    virtual std::any visit(DeclRefExpr*) = 0;
    virtual std::any visit(ParenExpr*) = 0;
    virtual std::any visit(BinaryOpExpr*) = 0;
    virtual std::any visit(ConditionalExpr*) = 0;
    virtual std::any visit(CompoundLiteralExpr*) = 0;
    virtual std::any visit(CastExpr*) = 0;
    virtual std::any visit(ArraySubscriptExpr*) = 0;
    virtual std::any visit(CallExpr*) = 0;
    virtual std::any visit(MemberExpr*) = 0;
    virtual std::any visit(UnaryOpExpr*) = 0;
};

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

    AstNode(NodeKind nk): m_kind(nk){}
    virtual ~AstNode() = default;
    virtual NodeKind getKind() const {return m_kind;}

    // 值估计接口：主要目的是在运行时对 AST 进行求值，得到具体的数值结果。TODO 暂时不实现
    // 语义分析模块接口
    // IR生成接口
    // 通用访问接口: 主要是提供dump等功能
    virtual std::any accept(ASTVisitor*) = 0;

private:
    NodeKind m_kind;
};

// 声明说明符
enum StorageClass 
{
    N_SCLASS,
    TYPEDEF =   (1 << 1),
    EXTERN =    (1 << 2),
    STATIC =    (1 << 3),
    AUTO =      (1 << 4),
    REGISTER =  (1 << 5),
    SC_MASK = TYPEDEF | EXTERN | STATIC | AUTO | REGISTER
};
enum TypeSpecifier 
{
    N_TSPEC = 0,
    VOID =     (1 << 1),
    CHAR =     (1 << 2),
    SHORT =    (1 << 3),
    INT =      (1 << 4),
    LONG =     (1 << 5),
    LONGLONG = (1 << 6),
    FLOAT =    (1 << 7),
    DOUBLE =   (1 << 8),
    SIGNED =   (1 << 9),
    UNSIGNED = (1 << 10),
    _BOOL =    (1 << 11),
    _COMPLEX = (1 << 12),
    STRUCTORUNION =   (1 << 13),
    ENUM =     (1 << 14),
    TYPEDEFNAME = (1 << 15),
};

enum TypeQualifier 
{
    N_TQUAL = 0,
    CONST =    (1 << 1),
    RESTRICT = (1 << 2),
    VOLATILE = (1 << 3),
    TQ_MASK = CONST | RESTRICT | VOLATILE
};
enum FuncSpecifier 
{
    N_FSPEC = 0,
    INLINE = (1 << 1),
    FS_MASK = INLINE
};

using DeclGroup = std::vector<Decl*>;

