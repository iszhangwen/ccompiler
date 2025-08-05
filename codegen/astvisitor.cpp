#include "astvisitor.h"
#include "analyzer.h"
#include "ast/type.h"
#include "ast/decl.h"
#include "ast/stmt.h"
#include "ast/expr.h"
#include <iostream>

void CodegenASTVisitor::visit(IntegerLiteral* c) 
{
    std::cout << " IntegerLiteral" << std::endl;
}
void CodegenASTVisitor::visit(FloatingLiteral* c) 
{
    std::cout << " FloatingLiteral" << std::endl;
}
void CodegenASTVisitor::visit(CharacterLiteral* c) 
{
    std::cout << " CharacterLiteral" << std::endl;
}
void CodegenASTVisitor::visit(StringLiteral* c) 
{
    std::cout << " StringLiteral" << std::endl;
}
void CodegenASTVisitor::visit(DeclRefExpr* dre) 
{
    std::cout << " DeclRefExpr" << std::endl;
}
void CodegenASTVisitor::visit(ParenExpr* pe) 
{
    std::cout << " ParenExpr" << std::endl;
}
void CodegenASTVisitor::visit(BinaryOpExpr* boe) 
{
    std::cout << " BinaryOpExpr" << std::endl;
}
void CodegenASTVisitor::visit(ConditionalExpr* ce) 
{
    std::cout << " ConditionalExpr" << std::endl;
}
void CodegenASTVisitor::visit(CompoundLiteralExpr* cle) 
{
    std::cout << " CompoundLiteralExpr" << std::endl;
}
void CodegenASTVisitor::visit(CastExpr* ce) 
{
    std::cout << " CastExpr" << std::endl;
}
void CodegenASTVisitor::visit(ArraySubscriptExpr* ase) 
{
    std::cout << " ArraySubscriptExpr" << std::endl;
}
void CodegenASTVisitor::visit(CallExpr* ce) 
{
    std::cout << " CallExpr" << std::endl;
}
void CodegenASTVisitor::visit(MemberExpr* me) 
{
    std::cout << " MemberExpr" << std::endl;
}
void CodegenASTVisitor::visit(UnaryOpExpr* uoe) 
{
    std::cout << " UnaryOpExpr" << std::endl;
}
/*-----------------------Declarations node----------------------------------*/
void CodegenASTVisitor::visit(TranslationUnitDecl* ld) 
{
    std::cout << " TranslationUnitDecl" << std::endl;
    std::cout << " Decls size: " << ld->size() << std::endl;
    for (auto dc : ld->getDecls()) {
        if (dc) {
            dc->accept(this);
        }
    }
}
void CodegenASTVisitor::visit(LabelDecl* ld) 
{
    std::cout << " LabelDecl" << std::endl;
}
void CodegenASTVisitor::visit(ValueDecl* vd) 
{
    std::cout << " ValueDecl" << std::endl;
}
void CodegenASTVisitor::visit(DeclaratorDecl* dd) 
{
    std::cout << " DeclaratorDecl" << std::endl;
}
void CodegenASTVisitor::visit(VarDecl* vd) 
{
    std::cout << " VarDecl" << std::endl;
}
void CodegenASTVisitor::visit(ParmVarDecl* pvd) 
{
    std::cout << " ParmVarDecl" << std::endl;
}
void CodegenASTVisitor::visit(FunctionDecl* fd) 
{
    std::cout << " FunctionDecl" << std::endl;
}
void CodegenASTVisitor::visit(FieldDecl* fd) 
{
    std::cout << " FieldDecl" << std::endl;
}
void CodegenASTVisitor::visit(EnumConstantDecl* ecd) 
{
    std::cout << " EnumConstantDecl" << std::endl;
}
void CodegenASTVisitor::visit(IndirectFieldDecl* ifd) 
{
    std::cout << " IndirectFieldDecl" << std::endl;
}
void CodegenASTVisitor::visit(TypedefDecl* tnd) 
{
    std::cout << " TypedefDecl" << std::endl;
}
void CodegenASTVisitor::visit(EnumDecl* ed) 
{
    std::cout << " EnumDecl" << std::endl;
}
void CodegenASTVisitor::visit(RecordDecl* rd)  
{
    std::cout << " RecordDecl" << std::endl;
}
/*-----------------------statemnts node----------------------------------*/
void CodegenASTVisitor::visit(LabelStmt* ls) 
{
    std::cout << " LabelStmt" << std::endl;
}
void CodegenASTVisitor::visit(CaseStmt* cs) 
{
    std::cout << " CaseStmt" << std::endl;
}
void CodegenASTVisitor::visit(DefaultStmt* ds) 
{
    std::cout << " DefaultStmt" << std::endl;
}
void CodegenASTVisitor::visit(CompoundStmt* cs) 
{
    std::cout << " CompoundStmt" << std::endl;
}
void CodegenASTVisitor::visit(DeclStmt* ds) 
{
    std::cout << " DeclStmt" << std::endl;
}
void CodegenASTVisitor::visit(ExprStmt* es) 
{
    std::cout << " ExprStmt" << std::endl;
}
void CodegenASTVisitor::visit(IfStmt* is) 
{
    std::cout << " IfStmt" << std::endl;
}
void CodegenASTVisitor::visit(SwitchStmt* ss) 
{
    std::cout << " SwitchStmt" << std::endl;
}
void CodegenASTVisitor::visit(WhileStmt* ws) 
{
    std::cout << " WhileStmt" << std::endl;
}
void CodegenASTVisitor::visit(DoStmt* ds) 
{
    std::cout << " DoStmt" << std::endl;
}
void CodegenASTVisitor::visit(ForStmt* fs) 
{
    std::cout << " ForStmt" << std::endl;
}
void CodegenASTVisitor::visit(GotoStmt* gs) 
{
    std::cout << " GotoStmt" << std::endl;
}
void CodegenASTVisitor::visit(ContinueStmt* cs) 
{
    std::cout << " ContinueStmt" << std::endl;
}
void CodegenASTVisitor::visit(BreakStmt* bs) 
{
    std::cout << " BreakStmt" << std::endl;
}
void CodegenASTVisitor::visit(ReturnStmt* rs)     
{
    std::cout << " ReturnStmt" << std::endl;
}