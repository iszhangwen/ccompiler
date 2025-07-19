#pragma once
#include <string>
#include <ast/decl.h>
#include <ast/stmt.h>
#include <ast/expr.h>
#include <ast/type.h>
#include <scanner.h>
#include <scope.h>
#include <sema.h>

// 状态机解析type specifier
class ParseTypeSpec {
public:
    enum TSState {
        START,
        FOUND_SIGNED_UNSIGNED,
        FOUND_SHORT,
        FOUND_LONG,
        FOUND_LONG2,
        FOUND_TYPE,
        ERROR
    };
    void operator()(TSState& curState, TokenKind cond, int& ts);
};

class Parser {
private:
    using DeclList = std::vector<Decl*>;
    Scope *curScope_;
    Source *buf_;
    TokenSequence *seq_;
    TranslationUnitDecl* unit_;
    sema* sema_;

    /*
    错误处理策略，遇到错误是中止编译程序
    */
    void error(const std::string& val);
    void error(Token *tk, const std::string& val);

    // 作用域栈函数
    void enterScope(Scope::ScopeType);
    void exitScope();
    bool isTypeName(Token*);

    /*-----------------------------Expressions-----------------------------------------*/
    Expr* parsePrimaryExpr();
    Expr* parsePostfixExpr();
    Expr* parseArgListExpr();
    Expr* parseUnaryExpr();
    Expr* parseCastExpr();
    Expr* parseMultiExpr();
    Expr* parseAddExpr();
    Expr* parseShiftExpr();
    Expr* parseRelationalExpr();
    Expr* parseEqualExpr();
    Expr* parseBitANDExpr();
    Expr* parseBitXORExpr();
    Expr* parseBitORExpr();
    Expr* parseLogicalANDExpr();
    Expr* parseLogicalORExpr();
    Expr* parseConditionalExpr();
    Expr* parseAssignExpr();
    Expr* parseExpr();
    Expr* parseConstansExpr();
    // 该表达式不是文法符号，是为了简化分析歧义
    Expr* parseParenExpr(); 
    /*-------------------------------Declarations--------------------------------------*/
    using DeclaratorGroup = std::vector<Declarator>;
    // 6.7 declaration
    DeclGroup parseDeclaration();
    QualType parseDeclarationSpec(int* sc, int* fs);
    Decl* parseInitDeclarator(QualType qt, int sc, int fs);
    // 6.7.1 storage-class-specifier
    void parseStorageClassSpec(int* sc, TokenKind);
    // 6.7.2 type-specifier
    void parseTypeSpec(int*, TokenKind, ParseTypeSpec::TSState);
    QualType parseStructOrUnionSpec(TokenKind);
    DeclGroup parseStructDeclarationList();
    QualType parseSpecQualList();
    DeclGroup parseStructDeclaratorList(QualType qt);
    Decl* parseStructDeclarator(QualType qt);
    QualType parseEnumSpec();
    void parseEnumerator();
    // 6.7.3 type-qualifier
    void parseTypeQualList(int*, TokenKind);
    // 6.7.4 function-specifier
    void parseFunctionSpec(int* fs, TokenKind);
    // 6.7.5 declarator
    Decl* parseDeclarator(QualType qt, int sc, int fs);
    void parseDirectDeclarator();
    void parsePointer();
    void parseParameterTypeList();
    void parseParameterList();
    void parseParameterDeclaration();
    void parseIdentifierList();
    // 6.7.6 type-name
    void parseTypeName();
    void parseAbstractDeclarator();
    void parseDirectAbstractDeclarator();
    // 6.7.7 typedef-name
    void parseTypedefName();
    // 6.7.8 initializer
    void parseInitializer();
    void parseInitializerList();
    void parseDesignation();
    void parseDesignator();
    /*--------------------------------Statements---------------------------------------*/
    Stmt* parseStmt();
    Stmt* parseLabeledStmt();
    Stmt* parseCompoundStmt();
    Stmt* parseExprStmt();
    Stmt* parseSelectionStmt();
    Stmt* parseIterationStmt();
    Stmt* parseJumpStmt();
    /*-------------------------------External definitions-------------------------------*/
    std::vector<Decl*> parseExternalDeclaration();

public:
    explicit Parser(const std::string& filename);
    virtual ~Parser();
    // 解析根节点
    void parseTranslationUnit();
};