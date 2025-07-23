#pragma once

#include <string>
#include <vector>
#include "ast/ast.h"

class Scope;
class Symbol; 
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
class SemaAnalyzer
{
public:
    /*-----------------------type----------------------------------*/
    Type* onActBuiltinType(int); 
    Type* onActDerivedType();
    Type* onActPointerType(QualType);
    Type* onActArrayType();
    Type* onActFunctionType();
    Type* onActTagType();
    Type* onActRecordType(bool, Decl*);
    Type* onActEnumType();
    Type* onActTypedefType();
    /*-----------------------expression node----------------------------------*/
    Expr* onActConstant(Token*); 
    Expr* onActDeclRefExpr(Token*);
    Expr* onActParenExpr(Expr*);
    Expr* onActBinaryOpExpr(Expr*, Expr*, int);
    Expr* onActConditionalExpr(Expr*, Expr*, Expr*);;
    Expr* onActCompoundLiteralExpr();
    Expr* onActCastExpr(QualType&, Expr*);
    Expr* onActArraySubscriptExpr(Expr*,Expr*);
    Expr* onActCallExpr(Expr*, Expr*);
    Expr* onActMemberExpr(Expr*, Expr*, bool);
    Expr* onActUnaryOpExpr(Expr*, int);
    /*-----------------------Declarations node----------------------------------*/
    TranslationUnitDecl* onActTranslationUnitDecl(DeclGroup);
    Decl* onActNamedDecl();
    Decl* onActLabelDecl();
    Decl* onActValueDecl();
    Decl* onActDeclaratorDecl();
    Decl* onActVarDecl();
    Decl* onActParmVarDecl();
    Decl* onActFunctionDecl(); 
    Decl* onActFieldDecl();
    Decl* onActEnumConstantDecl();
    Decl* onActIndirectFieldDecl(); 
    Decl* onActTypeDecl();
    Decl* onActTypedefNameDecl();
    Decl* onActTypedefDecl();
    Decl* onActTagDecl();
    Decl* onActEnumDecl(); 
    Decl* onActRecordDecl(Symbol* id, bool isDefinition, bool isStruct);
    /*-----------------------statemnts node----------------------------------*/
    Stmt* onActLabelStmt();
    Stmt* onActCaseStmt();
    Stmt* onActDefaultStmt();
    Stmt* onActCompoundStmt(); 
    Stmt* onActDeclStmt();
    Stmt* onActExprStmt();
    Stmt* onActIfStmt();
    Stmt* onActSwitchStmt();
    Stmt* onActWhileStmt();
    Stmt* onActDoStmt();
    Stmt* onActForStmt();
    Stmt* onActGotoStmt(); 
    Stmt* onActContinueStmt();
    Stmt* onActBreakStmt();
    Stmt* onActReturnStmt(); 
};