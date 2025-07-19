#pragma once

#include <string>
#include <vector>

#define RE_TEST {return nullptr;}

class Scope; 
class Decl;
class TranslationUnitDecl;
class FunctionDecl;
class VarDecl;
class Type;
class QualType;
class Expr;
class Stmt;
class Token;

// 使用语法制导翻译方案，翻译过程即完成语义检查过程
// 语义分析主要有两个作用：AST构建，和语义分析
class sema
{
public:
    /*-----------------------type----------------------------------*/
    Type* onACtBuiltinType() RE_TEST
    Type* onACtDerivedType() RE_TEST
    Type* onACtPointerType() RE_TEST
    Type* onACtArrayType() RE_TEST
    Type* onACtFunctionType() RE_TEST
    Type* onACtTagType() RE_TEST
    Type* onACtRecordType(int, Decl*) RE_TEST
    Type* onACtEnumType() RE_TEST
    Type* onACtTypedefType() RE_TEST
    /*-----------------------expression node----------------------------------*/
    Expr* onActConstant(Token*) RE_TEST
    Expr* onActDeclRefExpr(Token*) RE_TEST
    Expr* onActParenExpr(Expr*) RE_TEST
    Expr* onActBinaryOpExpr(Expr*, Expr*, int) RE_TEST
    Expr* onActConditionalExpr(Expr*, Expr*, Expr*) RE_TEST
    Expr* onActCompoundLiteralExpr() RE_TEST
    Expr* onActCastExpr(QualType&, Expr*) RE_TEST
    Expr* onActArraySubscriptExpr(Expr*,Expr*) RE_TEST
    Expr* onActCallExpr(Expr*, Expr*) RE_TEST
    Expr* onActMemberExpr(Expr*, Expr*, bool) RE_TEST
    Expr* onActUnaryOpExpr(Expr*, int) RE_TEST
    /*-----------------------Declarations node----------------------------------*/
    Decl* onActTranslationUnitDecl() RE_TEST
    Decl* onActNamedDecl() RE_TEST
    Decl* onActLabelDecl() RE_TEST
    Decl* onActValueDecl() RE_TEST
    Decl* onActDeclaratorDecl() RE_TEST
    Decl* onActVarDecl() RE_TEST
    Decl* onActParmVarDecl() RE_TEST
    Decl* onActFunctionDecl() RE_TEST
    Decl* onActFieldDecl() RE_TEST
    Decl* onActEnumConstantDecl() RE_TEST
    Decl* onActIndirectFieldDecl() RE_TEST
    Decl* onActTypeDecl() RE_TEST
    Decl* onActTypedefNameDecl() RE_TEST
    Decl* onActTypedefDecl() RE_TEST
    Decl* onActTagDecl() RE_TEST
    Decl* onActEnumDecl() RE_TEST
    Decl* onActStructDecl(IdentifierInfo, DeclGroup) RE_TEST
    Decl* onActUnionDecl() RE_TEST
    /*-----------------------statemnts node----------------------------------*/
    Stmt* onActLabelStmt() RE_TEST
    Stmt* onActCaseStmt() RE_TEST
    Stmt* onActDefaultStmt() RE_TEST
    Stmt* onActCompoundStmt() RE_TEST
    Stmt* onActDeclStmt() RE_TEST
    Stmt* onActExprStmt() RE_TEST
    Stmt* onActIfStmt() RE_TEST
    Stmt* onActSwitchStmt() RE_TEST
    Stmt* onActWhileStmt() RE_TEST
    Stmt* onActDoStmt() RE_TEST
    Stmt* onActForStmt()RE_TEST
    Stmt* onActGotoStmt() RE_TEST
    Stmt* onActContinueStmt() RE_TEST
    Stmt* onActBreakStmt() RE_TEST
    Stmt* onActReturnStmt() RE_TEST
};