#pragma once
#include "ast.h"
#include "usedef.h"

#include <stack>
#include <unordered_map>
#include <any>

class Module;
class Function;
class BasicBlock;
class LValue;

class IRBuilder : public ASTVisitor
{
public:
    IRBuilder();
    std::any visit(TranslationUnitDecl* ptr);
    std::any visit(LabelDecl* ptr);
    std::any visit(ValueDecl* ptr);
    std::any visit(DeclaratorDecl* ptr);
    std::any visit(VarDecl* ptr);
    std::any visit(ParmVarDecl* ptr);
    std::any visit(FunctionDecl* ptr);
    std::any visit(FieldDecl* ptr);
    std::any visit(EnumConstantDecl* ptr);
    std::any visit(TypedefDecl* ptr);
    std::any visit(EnumDecl* ptr);
    std::any visit(RecordDecl* ptr);
    // 访问语句节点
    std::any visit(LabelStmt* ptr);
    std::any visit(CaseStmt* ptr);
    std::any visit(DefaultStmt* ptr);
    std::any visit(CompoundStmt* ptr);
    std::any visit(DeclStmt* ptr);
    std::any visit(ExprStmt* ptr);
    std::any visit(IfStmt* ptr);
    std::any visit(SwitchStmt* ptr);
    std::any visit(WhileStmt* ptr);
    std::any visit(DoStmt* ptr);
    std::any visit(ForStmt* ptr);
    std::any visit(GotoStmt* ptr);
    std::any visit(ContinueStmt* ptr);
    std::any visit(BreakStmt* ptr);
    std::any visit(ReturnStmt* ptr);
    // 访问表达式节点
    std::any visit(IntegerLiteral* ptr);
    std::any visit(FloatingLiteral* ptr);
    std::any visit(CharacterLiteral* ptr);
    std::any visit(StringLiteral* ptr);
    std::any visit(DeclRefExpr* ptr);
    std::any visit(ParenExpr* ptr);
    std::any visit(BinaryOpExpr* ptr);
    std::any visit(ConditionalExpr* ptr);
    std::any visit(CompoundLiteralExpr* ptr);
    std::any visit(CastExpr* ptr);
    std::any visit(ArraySubscriptExpr* ptr);
    std::any visit(CallExpr* ptr);
    std::any visit(MemberExpr* ptr);
    std::any visit(UnaryOpExpr* ptr);
    void dump();

private:
    Module* m_module;
    Module* m_curFuntion;
    std::unordered_map<NamedDecl*, Value*> m_nameDeclAddr;
};

