#include "astdump.h"
#include "type.h"
#include "decl.h"
#include "stmt.h"
#include "expr.h"
#include "sema.h"

void DumpAstVisitor::visit(IntegerLiteral* c) 
{
    DumpAst dast(this);
    ss_ << " IntegerLiteral" << "\n";
}
void DumpAstVisitor::visit(FloatingLiteral* c) 
{
    DumpAst dast(this);
    ss_ << " FloatingLiteral" << "\n";
}
void DumpAstVisitor::visit(CharacterLiteral* c) 
{
    DumpAst dast(this);
    ss_  << " CharacterLiteral" << "\n";
}
void DumpAstVisitor::visit(StringLiteral* c) 
{
    DumpAst dast(this);
    ss_ << " StringLiteral" << "\n";
}
void DumpAstVisitor::visit(DeclRefExpr* dre) 
{
    DumpAst dast(this);
    ss_ << " DeclRefExpr" << "\n";
}
void DumpAstVisitor::visit(ParenExpr* pe) 
{
    DumpAst dast(this);
    ss_ << " ParenExpr" << "\n";
}
void DumpAstVisitor::visit(BinaryOpExpr* boe) 
{
    DumpAst dast(this);
    ss_ << " BinaryOpExpr" << "\n";
}
void DumpAstVisitor::visit(ConditionalExpr* ce) 
{
    DumpAst dast(this);
    ss_ << " ConditionalExpr" << "\n";
}
void DumpAstVisitor::visit(CompoundLiteralExpr* cle) 
{
    DumpAst dast(this);
    ss_ << " CompoundLiteralExpr" << "\n";
}
void DumpAstVisitor::visit(CastExpr* ce) 
{
    DumpAst dast(this);
    ss_ << " CastExpr" << "\n";
}
void DumpAstVisitor::visit(ArraySubscriptExpr* ase) 
{
    DumpAst dast(this);
    ss_ << " ArraySubscriptExpr" << "\n";
}
void DumpAstVisitor::visit(CallExpr* ce) 
{
    DumpAst dast(this);
    ss_ << " CallExpr" << "\n";
}
void DumpAstVisitor::visit(MemberExpr* me) 
{
    DumpAst dast(this);
    ss_ << " MemberExpr" << "\n";
}
void DumpAstVisitor::visit(UnaryOpExpr* uoe) 
{
    DumpAst dast(this);
    ss_ << " UnaryOpExpr" << "\n";
}
/*-----------------------Declarations node----------------------------------*/
void DumpAstVisitor::visit(TranslationUnitDecl* ld) 
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
void DumpAstVisitor::visit(LabelDecl* ld) 
{
    DumpAst dast(this);
    ss_ << " LabelDecl" << "\n";
}
void DumpAstVisitor::visit(ValueDecl* vd) 
{
    DumpAst dast(this);
    ss_ << " ValueDecl" << "\n";
}
void DumpAstVisitor::visit(DeclaratorDecl* dd) 
{
    DumpAst dast(this);
    ss_ << " DeclaratorDecl" << "\n";
}
void DumpAstVisitor::visit(VarDecl* vd) 
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
void DumpAstVisitor::visit(ParmVarDecl* pvd) 
{
    DumpAst dast(this);
    ss_  << " ParmVarDecl" << "\n";
}
void DumpAstVisitor::visit(FunctionDecl* fd) 
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
void DumpAstVisitor::visit(FieldDecl* fd) 
{
    DumpAst dast(this);
    ss_ << " FieldDecl" << "\n";
}
void DumpAstVisitor::visit(EnumConstantDecl* ecd) 
{
    DumpAst dast(this);
    ss_ << " EnumConstantDecl" << "\n";
}
void DumpAstVisitor::visit(IndirectFieldDecl* ifd) 
{
    DumpAst dast(this);
    ss_ << " IndirectFieldDecl" << "\n";
}
void DumpAstVisitor::visit(TypedefDecl* tnd) 
{
    DumpAst dast(this);
    ss_ << " TypedefDecl" << "\n";
}
void DumpAstVisitor::visit(EnumDecl* ed) 
{
    DumpAst dast(this);
    ss_ << " EnumDecl" << "\n";
}
void DumpAstVisitor::visit(RecordDecl* rd)  
{
    DumpAst dast(this);
    ss_ << " RecordDecl" << "\n";
}
/*-----------------------statemnts node----------------------------------*/
void DumpAstVisitor::visit(LabelStmt* ls) 
{
    DumpAst dast(this);
    ss_ << " LabelStmt" << "\n";
    if (ls) {
        if (ls->getStmt()) {
            ls->getStmt()->accept(this);
        }
    }
}
void DumpAstVisitor::visit(CaseStmt* cs) 
{
    DumpAst dast(this);
    ss_ << " CaseStmt" << "\n";
    if (cs) {
        if (cs->getCond()) {
            cs->getCond()->accept(this);
        }
        if (cs->getStmt()) {
            cs->getStmt()->accept(this);
        }
    }
}
void DumpAstVisitor::visit(DefaultStmt* ds) 
{
    DumpAst dast(this);
    ss_ << " DefaultStmt" << "\n";
    if (ds) {
        if (ds->getStmt()) {
            ds->getStmt()->accept(this);
        }
    }
}
void DumpAstVisitor::visit(CompoundStmt* cs) 
{
    DumpAst dast(this);
    ss_ << " CompoundStmt" << "\n";
    if (cs) {
        for (auto p : cs->getStmts()) {
            if (p) {
                p->accept(this);
            }
        }
    }
}
void DumpAstVisitor::visit(DeclStmt* ds) 
{
    DumpAst dast(this);
    ss_ << " DeclStmt" << "\n";
    level++;
    if (ds->getDecl()) {
        ds->getDecl()->accept(this);
    }
    level--;
}
void DumpAstVisitor::visit(ExprStmt* es) 
{
    DumpAst dast(this);
    ss_  << " ExprStmt" << "\n";
}
void DumpAstVisitor::visit(IfStmt* is) 
{
    DumpAst dast(this);
    ss_ << " IfStmt" << "\n";
    if (is) {
        if (is->getCond()) {
            is->getCond()->accept(this);
        }
        if (is->getThen()) {
            is->getThen()->accept(this);
        }
        if (is->getElse()) {
            is->getElse()->accept(this);
        }
    }
}
void DumpAstVisitor::visit(SwitchStmt* ss) 
{
    DumpAst dast(this);
    ss_ << " SwitchStmt" << "\n";
    if (ss) {
        if (ss->getCond()) {
            ss->getCond()->accept(this);
        }
        if (ss->getStmt()) {
            ss->getStmt()->accept(this);
        }
    }
}
void DumpAstVisitor::visit(WhileStmt* ws) 
{
    DumpAst dast(this);
    ss_ << " WhileStmt" << "\n";
}
void DumpAstVisitor::visit(DoStmt* ds) 
{
    DumpAst dast(this);
    ss_ << " DoStmt" << "\n";
}
void DumpAstVisitor::visit(ForStmt* fs) 
{
    DumpAst dast(this);
    ss_ << " ForStmt" << "\n";
}
void DumpAstVisitor::visit(GotoStmt* gs) 
{
    DumpAst dast(this);
    ss_ << " GotoStmt" << "\n";
    if (gs) {
        if (gs->getLabel()) {
            gs->getLabel()->accept(this);
        }
    }
}
void DumpAstVisitor::visit(ContinueStmt* cs) 
{
    DumpAst dast(this);
    ss_ << " ContinueStmt" << "\n";
}
void DumpAstVisitor::visit(BreakStmt* bs) 
{
    DumpAst dast(this);
    ss_ << " BreakStmt" << "\n";
}
void DumpAstVisitor::visit(ReturnStmt* rs)     
{
    DumpAst dast(this);
    ss_  << " ReturnStmt" << "\n";
}