#pragma once

#include <string>
#include <vector>
#include "ast/ast.h"


// 使用语法制导翻译方案，翻译过程即完成语义检查过程
// 语义分析主要有两个作用：AST构建，和语义分析
class SemaAnalyzer : public ASTVisitor 
{
private:
    SymbolTableContext* sys_; // 符号表上下文
public:
    SemaAnalyzer(SymbolTableContext* sys): sys_(sys) {}
    /*-----------------------expression node----------------------------------*/
    virtual void visit(IntegerLiteral* c) override;
    virtual void visit(FloatingLiteral* c) override;
    virtual void visit(CharacterLiteral* c) override;
    virtual void visit(StringLiteral* c) override;
    virtual void visit(DeclRefExpr* dre) override;
    virtual void visit(ParenExpr* pe) override;
    virtual void visit(BinaryOpExpr* boe) override;
    virtual void visit(ConditionalExpr* ce) override;
    virtual void visit(CompoundLiteralExpr* cle) override;
    virtual void visit(CastExpr* ce) override;
    virtual void visit(ArraySubscriptExpr* ase) override;
    virtual void visit(CallExpr* ce) override;
    virtual void visit(MemberExpr* me) override;
    virtual void visit(UnaryOpExpr* uoe) override;
    /*-----------------------Declarations node----------------------------------*/
    virtual void visit(TranslationUnitDecl* tud) override;
    virtual void visit(LabelDecl* ld) override;
    virtual void visit(ValueDecl* vd) override;
    virtual void visit(DeclaratorDecl* dd) override;
    virtual void visit(VarDecl* vd) override;
    virtual void visit(ParmVarDecl* pvd) override;
    virtual void visit(FunctionDecl* fd) override;
    virtual void visit(FieldDecl* fd) override;
    virtual void visit(EnumConstantDecl* ecd) override;
    virtual void visit(IndirectFieldDecl* ifd) override;
    virtual void visit(TypedefDecl* tnd) override;
    virtual void visit(EnumDecl* ed) override;
    virtual void visit(RecordDecl* rd) override;
    /*-----------------------statemnts node----------------------------------*/
    virtual void visit(LabelStmt* ls) override;
    virtual void visit(CaseStmt* cs) override;
    virtual void visit(DefaultStmt* ds) override;
    virtual void visit(CompoundStmt* cs) override;
    virtual void visit(DeclStmt* ds) override;
    virtual void visit(ExprStmt* es) override;
    virtual void visit(IfStmt* is) override;
    virtual void visit(SwitchStmt* ss) override;
    virtual void visit(WhileStmt* ws) override;
    virtual void visit(DoStmt* ds) override;
    virtual void visit(ForStmt* fs) override;
    virtual void visit(GotoStmt* gs) override;
    virtual void visit(ContinueStmt* cs) override;
    virtual void visit(BreakStmt* bs) override;
    virtual void visit(ReturnStmt* rs) override;    
};