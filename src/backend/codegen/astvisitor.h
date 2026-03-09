#pragma once

#include <string>
#include <vector>
#include "ast.h"

// 使用语法制导翻译方案，翻译过程即完成语义检查过程
// 语义分析主要有两个作用：AST构建，和语义分析
class CodegenASTVisitor : public ASTVisitor 
{
public:
    /*-----------------------expression node----------------------------------*/
    virtual std::any  visit(IntegerLiteral* c) override;
    virtual std::any  visit(FloatingLiteral* c) override;
    virtual std::any  visit(CharacterLiteral* c) override;
    virtual std::any  visit(StringLiteral* c) override;
    virtual std::any  visit(DeclRefExpr* dre) override;
    virtual std::any  visit(ParenExpr* pe) override;
    virtual std::any  visit(BinaryOpExpr* boe) override;
    virtual std::any  visit(ConditionalExpr* ce) override;
    virtual std::any  visit(CompoundLiteralExpr* cle) override;
    virtual std::any  visit(CastExpr* ce) override;
    virtual std::any  visit(ArraySubscriptExpr* ase) override;
    virtual std::any  visit(CallExpr* ce) override;
    virtual std::any  visit(MemberExpr* me) override;
    virtual std::any  visit(UnaryOpExpr* uoe) override;
    /*-----------------------Declarations node----------------------------------*/
    virtual std::any  visit(TranslationUnitDecl* ld) override;
    virtual std::any  visit(LabelDecl* ld) override;
    virtual std::any  visit(ValueDecl* vd) override;
    virtual std::any  visit(DeclaratorDecl* dd) override;
    virtual std::any  visit(VarDecl* vd) override;
    virtual std::any  visit(ParmVarDecl* pvd) override;
    virtual std::any  visit(FunctionDecl* fd) override;
    virtual std::any  visit(FieldDecl* fd) override;
    virtual std::any  visit(EnumConstantDecl* ecd) override;
    virtual std::any  visit(TypedefDecl* tnd) override;
    virtual std::any  visit(EnumDecl* ed) override;
    virtual std::any  visit(RecordDecl* rd) override; 
    /*-----------------------statemnts node----------------------------------*/
    virtual std::any  visit(LabelStmt* ls) override;
    virtual std::any  visit(CaseStmt* cs) override;
    virtual std::any  visit(DefaultStmt* ds) override;
    virtual std::any  visit(CompoundStmt* cs) override;
    virtual std::any  visit(DeclStmt* ds) override;
    virtual std::any  visit(ExprStmt* es) override;
    virtual std::any  visit(IfStmt* is) override;
    virtual std::any  visit(SwitchStmt* ss) override;
    virtual std::any  visit(WhileStmt* ws) override;
    virtual std::any  visit(DoStmt* ds) override;
    virtual std::any  visit(ForStmt* fs) override;
    virtual std::any  visit(GotoStmt* gs) override;
    virtual std::any  visit(ContinueStmt* cs) override;
    virtual std::any  visit(BreakStmt* bs) override;
    virtual std::any  visit(ReturnStmt* rs) override;    
};
