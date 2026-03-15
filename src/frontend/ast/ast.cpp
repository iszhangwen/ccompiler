#include "ast.h"
#include "decl.h"
#include "stmt.h"
#include "expr.h"
#include "type.h"

#include "analyzer.h"

#define DEFINE_ACCEPT_METHOD(CLASS_NAME, VISITOR_BASE, RETURN_TYPE) \
RETURN_TYPE CLASS_NAME::accept(VISITOR_BASE* vt) { \
    if (vt) { \
        return vt->visit(this); \
    } \
    return RETURN_TYPE(); \
}\

// 通用访问接口: 主要是提供dump等功能
DEFINE_ACCEPT_METHOD(TranslationUnitDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(LabelDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(ValueDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(DeclaratorDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(VarDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(ParmVarDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(FunctionDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(FieldDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(EnumConstantDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(EnumDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(RecordDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(TypedefDecl, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(LabelStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(CaseStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(DefaultStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(CompoundStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(DeclStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(ExprStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(IfStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(SwitchStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(WhileStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(DoStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(ForStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(GotoStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(ContinueStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(BreakStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(ReturnStmt, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(IntegerLiteral, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(CharacterLiteral, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(FloatingLiteral, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(StringLiteral, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(DeclRefExpr, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(ParenExpr, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(BinaryOpExpr, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(ConditionalExpr, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(CompoundLiteralExpr, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(CastExpr, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(ArraySubscriptExpr, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(CallExpr, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(MemberExpr, ASTVisitor, std::any)
DEFINE_ACCEPT_METHOD(UnaryOpExpr, ASTVisitor, std::any)