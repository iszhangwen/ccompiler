#include "astvisitor.h"
#include "analyzer.h"
#include "ast/type.h"
#include "ast/decl.h"
#include "ast/stmt.h"
#include "ast/expr.h"
#include "sema.h"
#include <iostream>

void CodegenASTVisitor::visit(IntegerLiteral* c) 
{
    DumpAst dast(this);
    ss_ << " IntegerLiteral" << "\n";
}
void CodegenASTVisitor::visit(FloatingLiteral* c) 
{
    DumpAst dast(this);
    ss_ << " FloatingLiteral" << "\n";
}
void CodegenASTVisitor::visit(CharacterLiteral* c) 
{
    DumpAst dast(this);
    ss_  << " CharacterLiteral" << "\n";
}
void CodegenASTVisitor::visit(StringLiteral* c) 
{
    DumpAst dast(this);
    ss_ << " StringLiteral" << "\n";
}
void CodegenASTVisitor::visit(DeclRefExpr* dre) 
{
    DumpAst dast(this);
    ss_ << " DeclRefExpr" << "\n";
}
void CodegenASTVisitor::visit(ParenExpr* pe) 
{
    DumpAst dast(this);
    ss_ << " ParenExpr" << "\n";
}
void CodegenASTVisitor::visit(BinaryOpExpr* boe) 
{
    DumpAst dast(this);
    ss_ << " BinaryOpExpr" << "\n";
}
void CodegenASTVisitor::visit(ConditionalExpr* ce) 
{
    DumpAst dast(this);
    ss_ << " ConditionalExpr" << "\n";
}
void CodegenASTVisitor::visit(CompoundLiteralExpr* cle) 
{
    DumpAst dast(this);
    ss_ << " CompoundLiteralExpr" << "\n";
}
void CodegenASTVisitor::visit(CastExpr* ce) 
{
    DumpAst dast(this);
    ss_ << " CastExpr" << "\n";
}
void CodegenASTVisitor::visit(ArraySubscriptExpr* ase) 
{
    DumpAst dast(this);
    ss_ << " ArraySubscriptExpr" << "\n";
}
void CodegenASTVisitor::visit(CallExpr* ce) 
{
    DumpAst dast(this);
    ss_ << " CallExpr" << "\n";
}
void CodegenASTVisitor::visit(MemberExpr* me) 
{
    DumpAst dast(this);
    ss_ << " MemberExpr" << "\n";
}
void CodegenASTVisitor::visit(UnaryOpExpr* uoe) 
{
    DumpAst dast(this);
    ss_ << " UnaryOpExpr" << "\n";
}
/*-----------------------Declarations node----------------------------------*/
void CodegenASTVisitor::visit(TranslationUnitDecl* ld) 
{
    ss_   << " TranslationUnitDecl" << "size = " << ld->size() << "\n";
    level++;
    for (auto dc : ld->getDecls()) {
        if (dc) {
            dc->accept(this);
        }
    }
    level--;
}
void CodegenASTVisitor::visit(LabelDecl* ld) 
{
    DumpAst dast(this);
    ss_ << " LabelDecl" << "\n";
}
void CodegenASTVisitor::visit(ValueDecl* vd) 
{
    DumpAst dast(this);
    ss_ << " ValueDecl" << "\n";
}
void CodegenASTVisitor::visit(DeclaratorDecl* dd) 
{
    DumpAst dast(this);
    ss_ << " DeclaratorDecl" << "\n";
}
void CodegenASTVisitor::visit(VarDecl* vd) 
{
    DumpAst dast(this);
    ss_ << " VarDecl" << "\n";
    level++;
    Expr* ex = vd->getInitExpr();
    if (ex) {
        ex->accept(this);
    }
    level--;
}
void CodegenASTVisitor::visit(ParmVarDecl* pvd) 
{
    DumpAst dast(this);
    ss_  << " ParmVarDecl" << "\n";
}
void CodegenASTVisitor::visit(FunctionDecl* fd) 
{
    DumpAst dast(this);
    ss_   << " FunctionDecl" << "\n";
    level++;
    for (auto param : fd->getParmVarDeclList()) {
        param->accept(this);
    }
    if (fd->getBody()) {
        level++;
        for (auto st : fd->getBody()->getStmts()) {
            st->accept(this);
        }
        level--;
    }

    level--;
}
void CodegenASTVisitor::visit(FieldDecl* fd) 
{
    DumpAst dast(this);
    ss_ << " FieldDecl" << "\n";
}
void CodegenASTVisitor::visit(EnumConstantDecl* ecd) 
{
    DumpAst dast(this);
    ss_ << " EnumConstantDecl" << "\n";
}
void CodegenASTVisitor::visit(IndirectFieldDecl* ifd) 
{
    DumpAst dast(this);
    ss_ << " IndirectFieldDecl" << "\n";
}
void CodegenASTVisitor::visit(TypedefDecl* tnd) 
{
    DumpAst dast(this);
    ss_ << " TypedefDecl" << "\n";
}
void CodegenASTVisitor::visit(EnumDecl* ed) 
{
    DumpAst dast(this);
    ss_ << " EnumDecl" << "\n";
}
void CodegenASTVisitor::visit(RecordDecl* rd)  
{
    DumpAst dast(this);
    ss_ << " RecordDecl" << "\n";
}
/*-----------------------statemnts node----------------------------------*/
void CodegenASTVisitor::visit(LabelStmt* ls) 
{
    DumpAst dast(this);
    ss_ << " LabelStmt" << "\n";
}
void CodegenASTVisitor::visit(CaseStmt* cs) 
{
    DumpAst dast(this);
    ss_ << " CaseStmt" << "\n";
}
void CodegenASTVisitor::visit(DefaultStmt* ds) 
{
    DumpAst dast(this);
    ss_ << " DefaultStmt" << "\n";
}
void CodegenASTVisitor::visit(CompoundStmt* cs) 
{
    DumpAst dast(this);
    ss_ << " CompoundStmt" << "\n";
}
void CodegenASTVisitor::visit(DeclStmt* ds) 
{
    DumpAst dast(this);
    ss_ << " DeclStmt" << "\n";
    level++;
    if (ds->getDecl()) {
        ds->getDecl()->accept(this);
    }
    level--;
}
void CodegenASTVisitor::visit(ExprStmt* es) 
{
    DumpAst dast(this);
    ss_  << " ExprStmt" << "\n";
}
void CodegenASTVisitor::visit(IfStmt* is) 
{
    DumpAst dast(this);
    ss_ << " IfStmt" << "\n";
}
void CodegenASTVisitor::visit(SwitchStmt* ss) 
{
    DumpAst dast(this);
    ss_ << " SwitchStmt" << "\n";
}
void CodegenASTVisitor::visit(WhileStmt* ws) 
{
    DumpAst dast(this);
    ss_ << " WhileStmt" << "\n";
}
void CodegenASTVisitor::visit(DoStmt* ds) 
{
    DumpAst dast(this);
    ss_ << " DoStmt" << "\n";
}
void CodegenASTVisitor::visit(ForStmt* fs) 
{
    DumpAst dast(this);
    ss_ << " ForStmt" << "\n";
}
void CodegenASTVisitor::visit(GotoStmt* gs) 
{
    DumpAst dast(this);
    ss_ << " GotoStmt" << "\n";
}
void CodegenASTVisitor::visit(ContinueStmt* cs) 
{
    DumpAst dast(this);
    ss_ << " ContinueStmt" << "\n";
}
void CodegenASTVisitor::visit(BreakStmt* bs) 
{
    DumpAst dast(this);
    ss_ << " BreakStmt" << "\n";
}
void CodegenASTVisitor::visit(ReturnStmt* rs)     
{
    DumpAst dast(this);
    ss_  << " ReturnStmt" << "\n";
}