#include "cfgbuilder.h"
#include <iostream>

CFGBuilder::CFGBuilder()
{
}

void CFGBuilder::visit(IntegerLiteral* c)
{
    std::cout << " IntegerLiteral" << std::endl;
}
void CFGBuilder::visit(FloatingLiteral* c)
{
    std::cout << " FloatingLiteral" << std::endl;
}
void CFGBuilder::visit(CharacterLiteral* c)
{
    std::cout << " CharacterLiteral" << std::endl;
}
void CFGBuilder::visit(StringLiteral* c)
{
    std::cout << " StringLiteral" << std::endl;
}
void CFGBuilder::visit(DeclRefExpr* dre)
{
    std::cout << " DeclRefExpr" << std::endl;
}
void CFGBuilder::visit(ParenExpr* pe)
{
    std::cout << " ParenExpr" << std::endl;
}
void CFGBuilder::visit(BinaryOpExpr* boe)
{
    std::cout << " BinaryOpExpr" << std::endl;
}
void CFGBuilder::visit(ConditionalExpr* ce)
{
    std::cout << " ConditionalExpr" << std::endl;
}
void CFGBuilder::visit(CompoundLiteralExpr* cle)
{
    std::cout << " CompoundLiteralExpr" << std::endl;
}
void CFGBuilder::visit(CastExpr* ce)
{
    std::cout << " CastExpr" << std::endl;
}
void CFGBuilder::visit(ArraySubscriptExpr* ase)
{
    std::cout << " ArraySubscriptExpr" << std::endl;
}
void CFGBuilder::visit(CallExpr* ce)
{
    std::cout << " CallExpr" << std::endl;
}
void CFGBuilder::visit(MemberExpr* me)
{
    std::cout << " MemberExpr" << std::endl;
}
void CFGBuilder::visit(UnaryOpExpr* uoe)
{
    std::cout << " UnaryOpExpr" << std::endl;
}
/*-----------------------Declarations node----------------------------------*/
void CFGBuilder::visit(TranslationUnitDecl* ld)
{
    std::cout << " TranslationUnitDecl" << std::endl;
}
void CFGBuilder::visit(LabelDecl* ld)
{
    std::cout << " LabelDecl" << std::endl;
}
void CFGBuilder::visit(ValueDecl* vd)
{
    std::cout << " ValueDecl" << std::endl;
}
void CFGBuilder::visit(DeclaratorDecl* dd)
{
    std::cout << " DeclaratorDecl" << std::endl;
}
void CFGBuilder::visit(VarDecl* vd)
{
    std::cout << " VarDecl" << std::endl;
}
void CFGBuilder::visit(ParmVarDecl* pvd)
{
    std::cout << " ParmVarDecl" << std::endl;
}
void CFGBuilder::visit(FunctionDecl* fd)
{
    std::cout << " FunctionDecl" << std::endl;
}
void CFGBuilder::visit(FieldDecl* fd)
{
    std::cout << " FieldDecl" << std::endl;
}
void CFGBuilder::visit(EnumConstantDecl* ecd)
{
    std::cout << " EnumConstantDecl" << std::endl;
}
void CFGBuilder::visit(TypedefDecl* tnd)
{
    std::cout << " TypedefDecl" << std::endl;
}
void CFGBuilder::visit(EnumDecl* ed)
{
    std::cout << " EnumDecl" << std::endl;
}
void CFGBuilder::visit(RecordDecl* rd)
{
    std::cout << " RecordDecl" << std::endl;
}
/*-----------------------statemnts node----------------------------------*/
void CFGBuilder::visit(LabelStmt* ls)
{
    std::cout << " LabelStmt" << std::endl;
}
void CFGBuilder::visit(CaseStmt* cs)
{
    std::cout << " CaseStmt" << std::endl;
}
void CFGBuilder::visit(DefaultStmt* ds)
{
    std::cout << " DefaultStmt" << std::endl;
}
void CFGBuilder::visit(CompoundStmt* cs)
{
    std::cout << " CompoundStmt" << std::endl;
}
void CFGBuilder::visit(DeclStmt* ds)
{
    std::cout << " DeclStmt" << std::endl;
}
void CFGBuilder::visit(ExprStmt* es)
{
    std::cout << " ExprStmt" << std::endl;
}
void CFGBuilder::visit(IfStmt* is)
{
    std::cout << " IfStmt" << std::endl;
}
void CFGBuilder::visit(SwitchStmt* ss)
{
    std::cout << " SwitchStmt" << std::endl;
}
void CFGBuilder::visit(WhileStmt* ws)
{
    std::cout << " WhileStmt" << std::endl;
}
void CFGBuilder::visit(DoStmt* ds)
{
    std::cout << " DoStmt" << std::endl;
}
void CFGBuilder::visit(ForStmt* fs)
{
    std::cout << " ForStmt" << std::endl;
}
void CFGBuilder::visit(GotoStmt* gs)
{
    std::cout << " GotoStmt" << std::endl;
}
void CFGBuilder::visit(ContinueStmt* cs)
{
    std::cout << " ContinueStmt" << std::endl;
}
void CFGBuilder::visit(BreakStmt* bs)
{
    std::cout << " BreakStmt" << std::endl;
}
void CFGBuilder::visit(ReturnStmt* rs)
{
    std::cout << " ReturnStmt" << std::endl;
}
