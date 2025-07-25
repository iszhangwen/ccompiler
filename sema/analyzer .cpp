#include "sema.h"
#include <ast/decl.h>
#include <ast/expr.h>
#include <ast/stmt.h>
#include <ast/type.h>

Type* SemaAnalyzer::onActBuiltinType(int ts) 
{
    return nullptr;
}

Type* SemaAnalyzer::onActDerivedType() 
{
    return nullptr;
}

Type* SemaAnalyzer::onActPointerType(QualType pointee) 
{
   return nullptr;
}

Type* SemaAnalyzer::onActArrayType() 
{
    return nullptr;
}

Type* SemaAnalyzer::onActFunctionType() 
{
    return nullptr;
}

Type* SemaAnalyzer::onActTagType() 
{
    return nullptr;
}

Type* SemaAnalyzer::onActRecordType(bool isStruct, Decl* dc) 
{
    return nullptr;
}

Type* SemaAnalyzer::onActEnumType() 
{
    return nullptr;
}

Type* SemaAnalyzer::onActTypedefType() 
{
    return nullptr;
}
/*-----------------------expression node----------------------------------*/
Expr* SemaAnalyzer::onActConstant(Token*) 
{
    return nullptr;
}

Expr* SemaAnalyzer::onActDeclRefExpr(Token*) 
{
    return nullptr;
}

Expr* SemaAnalyzer::onActParenExpr(Expr*) 
{
    return nullptr;
}

Expr* SemaAnalyzer::onActBinaryOpExpr(Expr*, Expr*, int) 
{
    return nullptr;
}

Expr* SemaAnalyzer::onActConditionalExpr(Expr*, Expr*, Expr*) 
{
    return nullptr;
}

Expr* SemaAnalyzer::onActCompoundLiteralExpr() 
{
    return nullptr;
}

Expr* SemaAnalyzer::onActCastExpr(QualType&, Expr*) 
{
    return nullptr;
}

Expr* SemaAnalyzer::onActArraySubscriptExpr(Expr*,Expr*) 
{
    return nullptr;
}

Expr* SemaAnalyzer::onActCallExpr(Expr*, Expr*) 
{
    return nullptr;
}

Expr* SemaAnalyzer::onActMemberExpr(Expr*, Expr*, bool) 
{
    return nullptr;
}

Expr* SemaAnalyzer::onActUnaryOpExpr(Expr*, int) 
{
    return nullptr;
}
/*-----------------------Declarations node----------------------------------*/
TranslationUnitDecl* SemaAnalyzer::onActTranslationUnitDecl(DeclGroup dc) 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActNamedDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActLabelDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActValueDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActDeclaratorDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActVarDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActParmVarDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActFunctionDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActFieldDecl(Symbol* id, QualType ty, Decl* parent, unsigned offset) 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActEnumConstantDecl(Symbol* id, Expr* ex) 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActIndirectFieldDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActTypeDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActTypedefNameDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActTypedefDecl() 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActEnumDecl(Symbol* id, bool isDefinition) 
{
    return nullptr;
}

Decl* SemaAnalyzer::onActRecordDecl(Symbol* id, bool isDefinition, bool isStruct) 
{
    return nullptr;
}
/*-----------------------statemnts node----------------------------------*/
Stmt* SemaAnalyzer::onActLabelStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActCaseStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActDefaultStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActCompoundStmt() 
{
    return nullptr;
}

Stmt* onActDeclStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActExprStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActIfStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActSwitchStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActWhileStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActDoStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActForStmt()
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActGotoStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActContinueStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActBreakStmt() 
{
    return nullptr;
}

Stmt* SemaAnalyzer::onActReturnStmt() 
{
    return nullptr;
}