#pragma once
#include <ast.h>
#include <expr.h>
#include <string>
#include "diag.h"
#include "symbol.h"

namespace ccompiler {

// 使用语法制导翻译方案，翻译过程即完成语义检查过程
// 语义分析主要有两个作用：AST构建，和语义分析
class SemaAnalyzer : public ASTVisitor
{
public:
    SemaAnalyzer(DiagnosticEngine* diag, SymbolTableContext* ctx);

    // 入口：分析整个翻译单元
    void run(TranslationUnitDecl* tu);

    // --- ASTVisitor 实现 ---
    std::any visit(TranslationUnitDecl*) override;
    std::any visit(LabelDecl*) override;
    std::any visit(ValueDecl*) override;
    std::any visit(DeclaratorDecl*) override;
    std::any visit(VarDecl*) override;
    std::any visit(ParmVarDecl*) override;
    std::any visit(FunctionDecl*) override;
    std::any visit(FieldDecl*) override;
    std::any visit(EnumConstantDecl*) override;
    std::any visit(TypedefDecl*) override;
    std::any visit(EnumDecl*) override;
    std::any visit(RecordDecl*) override;
    std::any visit(LabelStmt*) override;
    std::any visit(CaseStmt*) override;
    std::any visit(DefaultStmt*) override;
    std::any visit(CompoundStmt*) override;
    std::any visit(DeclStmt*) override;
    std::any visit(ExprStmt*) override;
    std::any visit(IfStmt*) override;
    std::any visit(SwitchStmt*) override;
    std::any visit(WhileStmt*) override;
    std::any visit(DoStmt*) override;
    std::any visit(ForStmt*) override;
    std::any visit(GotoStmt*) override;
    std::any visit(ContinueStmt*) override;
    std::any visit(BreakStmt*) override;
    std::any visit(ReturnStmt*) override;
    std::any visit(IntegerLiteral*) override;
    std::any visit(FloatingLiteral*) override;
    std::any visit(CharacterLiteral*) override;
    std::any visit(StringLiteral*) override;
    std::any visit(DeclRefExpr*) override;
    std::any visit(ParenExpr*) override;
    std::any visit(BinaryOpExpr*) override;
    std::any visit(ConditionalExpr*) override;
    std::any visit(CompoundLiteralExpr*) override;
    std::any visit(CastExpr*) override;
    std::any visit(ArraySubscriptExpr*) override;
    std::any visit(CallExpr*) override;
    std::any visit(MemberExpr*) override;
    std::any visit(UnaryOpExpr*) override;

    // --- 类型工具方法 ---

    // 检查表达式是否为左值 (6.3.2.1)
    bool isLValue(Expr* e);

    // 检查是否为可修改左值 (6.5.16)
    bool isModifiableLValue(Expr* e);

    // 整数提升: _Bool/char/short -> int (6.3.1.1)
    QualType getIntegerPromotionType(QualType ty);

    // 常用算术转换: 找到二元操作的公共实类型 (6.3.1.8)
    QualType getCommonRealType(QualType t1, QualType t2);

    // 检查类型兼容性（赋值兼容）
    bool isTypeCompatible(QualType src, QualType dst);

    // 插入隐式转换: 在 expr 外包装 CastExpr 替代原表达式
    Expr* insertImplicitConversion(Expr* expr, QualType targetType, CastExpr::CastKind kind);

    // 对二元操作的两个操作数执行常用算术转换
    void applyUsualArithmeticConversions(Expr*& lhs, Expr*& rhs);

    // 对操作数执行整数提升并返回转换后的表达式
    Expr* applyIntegerPromotions(Expr* expr);

private:
    DiagnosticEngine* m_diag;       // 诊断引擎
    SymbolTableContext* m_ctx;      // 符号表上下文

    // 上下文追踪
    std::string m_currentFuncName;  // 当前函数名称（用于错误报告）
    QualType m_currentRetType;      // 当前函数的返回类型
    int m_loopDepth;                // 循环嵌套深度（break/continue 检查）
    int m_switchDepth;              // switch 嵌套深度（case/default 检查）
};

} // namespace ccompiler
