#include "astvisitor.h"
#include "analyzer.h"
#include "type.h"
#include <iostream>

std::any  CodegenASTVisitor::visit(IntegerLiteral* c) 
{
    std::cout << " IntegerLiteral" << std::endl;
}
std::any  CodegenASTVisitor::visit(FloatingLiteral* c) 
{
    std::cout << " FloatingLiteral" << std::endl;
}
std::any  CodegenASTVisitor::visit(CharacterLiteral* c) 
{
    std::cout << " CharacterLiteral" << std::endl;
}
std::any  CodegenASTVisitor::visit(StringLiteral* c) 
{
    std::cout << " StringLiteral" << std::endl;
}
std::any  CodegenASTVisitor::visit(DeclRefExpr* dre) 
{
    std::cout << " DeclRefExpr" << std::endl;
}
std::any  CodegenASTVisitor::visit(ParenExpr* pe) 
{
    std::cout << " ParenExpr" << std::endl;
}
std::any  CodegenASTVisitor::visit(BinaryOpExpr* boe) 
{
    std::cout << " BinaryOpExpr" << std::endl;
}
std::any  CodegenASTVisitor::visit(ConditionalExpr* ce) 
{
    std::cout << " ConditionalExpr" << std::endl;
}
std::any  CodegenASTVisitor::visit(CompoundLiteralExpr* cle) 
{
    std::cout << " CompoundLiteralExpr" << std::endl;
}
std::any  CodegenASTVisitor::visit(CastExpr* ce) 
{
    std::cout << " CastExpr" << std::endl;
}
std::any  CodegenASTVisitor::visit(ArraySubscriptExpr* ase) 
{
    std::cout << " ArraySubscriptExpr" << std::endl;
}
std::any  CodegenASTVisitor::visit(CallExpr* ce) 
{
    std::cout << " CallExpr" << std::endl;
}
std::any  CodegenASTVisitor::visit(MemberExpr* me) 
{
    std::cout << " MemberExpr" << std::endl;
}
std::any  CodegenASTVisitor::visit(UnaryOpExpr* uoe) 
{
    std::cout << " UnaryOpExpr" << std::endl;
}
/*-----------------------Declarations node----------------------------------*/
std::any  CodegenASTVisitor::visit(TranslationUnitDecl* ld) 
{
    std::cout << " TranslationUnitDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(LabelDecl* ld) 
{
    std::cout << " LabelDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(ValueDecl* vd) 
{
    std::cout << " ValueDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(DeclaratorDecl* dd) 
{
    std::cout << " DeclaratorDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(VarDecl* vd) 
{
    std::cout << " VarDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(ParmVarDecl* pvd) 
{
    std::cout << " ParmVarDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(FunctionDecl* fd) 
{
    std::cout << " FunctionDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(FieldDecl* fd) 
{
    std::cout << " FieldDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(EnumConstantDecl* ecd) 
{
    std::cout << " EnumConstantDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(TypedefDecl* tnd) 
{
    std::cout << " TypedefDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(EnumDecl* ed) 
{
    std::cout << " EnumDecl" << std::endl;
}
std::any  CodegenASTVisitor::visit(RecordDecl* rd)  
{
    std::cout << " RecordDecl" << std::endl;
}
/*-----------------------statemnts node----------------------------------*/
std::any  CodegenASTVisitor::visit(LabelStmt* ls) 
{
    std::cout << " LabelStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(CaseStmt* cs) 
{
    std::cout << " CaseStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(DefaultStmt* ds) 
{
    std::cout << " DefaultStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(CompoundStmt* cs) 
{
    std::cout << " CompoundStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(DeclStmt* ds) 
{
    std::cout << " DeclStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(ExprStmt* es) 
{
    std::cout << " ExprStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(IfStmt* is) 
{
    std::cout << " IfStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(SwitchStmt* ss) 
{
    std::cout << " SwitchStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(WhileStmt* ws) 
{
    std::cout << " WhileStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(DoStmt* ds) 
{
    std::cout << " DoStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(ForStmt* fs) 
{
    std::cout << " ForStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(GotoStmt* gs) 
{
    std::cout << " GotoStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(ContinueStmt* cs) 
{
    std::cout << " ContinueStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(BreakStmt* bs) 
{
    std::cout << " BreakStmt" << std::endl;
}
std::any  CodegenASTVisitor::visit(ReturnStmt* rs)     
{
    std::cout << " ReturnStmt" << std::endl;
}
