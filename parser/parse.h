#pragma once
#include <string>
#include <ast/decl.h>
#include <ast/stmt.h>
#include <ast/expr.h>
#include <ast/type.h>
#include <scanner.h>
#include <sema.h>

class Parser;

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
        FOUND_UDT_TYPE,
        ERROR
    };
    void operator()(TSState& curState, int* ts, TokenKind cond);
};

// RAII实现作用域管理
class ScopeManager {
    Parser* parent_;
public:
    ScopeManager(Parser* p, Scope::ScopeType st);
    ~ScopeManager();
};

class Parser {
private:
    friend ScopeManager;
    Source *buf_;
    TokenSequence *seq_;
    TranslationUnitDecl* unit_;
    SemaAnalyzer* sema_;
    SymbolTableContext* sys_;

    // 错误处理策略，遇到错误时中止编译程序
    void error(const std::string& val);
    void error(Token *tk, const std::string& val);

    /*-----------------------------Expressions-----------------------------------------*/
    Expr* parsePrimaryExpr();
    Expr* parseGenericSelection();
    Expr* parseGenericAssociation();
    Expr* parsePostfixExpr();
    std::vector<Expr*> parseArgListExpr();
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
    // 6.7 declaration
    DeclGroup parseDeclaration();
    QualType parseDeclarationSpec(int* sc, int* fs);
    Decl* parseInitDeclarator(QualType qt, int sc, int fs);
    Declarator parseInitDeclarator(QualType qt, int sc, int fs);
    // 6.7.1 storage-class-specifier
    void parseStorageClassSpec(int* sc, TokenKind);
    // 6.7.2 type-specifier
    void parseTypeSpec(int*, TokenKind, ParseTypeSpec::TSState);
    Type* parseStructOrUnionSpec(bool isStruct);
    Type* parseStructDeclarationList(Symbol*);
    QualType parseSpecQualList();
    DeclGroup parseStructDeclaratorList(QualType qt, Decl* parent);
    Decl* parseStructDeclarator(QualType qt);
    Type* parseEnumSpec();
    Type* parseEnumeratorList(Symbol*, Type*);
    EnumConstantDecl* parseEnumerator(QualType qt);
    // 6.7.3 type-qualifier
    int parseTypeQualList();
    // 6.7.4 function-specifier
    void parseFunctionSpec(int* fs, TokenKind);
    // 6.7.5 declarator
    Decl* parseDeclarator(QualType qt, int sc, int fs);
    void parseDirectDeclarator();
    QualType parsePointer(QualType);
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
    Decl* parseFunctionDefinitionBody(Decl*);

public:
    explicit Parser(const std::string& filename);
    virtual ~Parser();
    // 解析根节点
    void parseTranslationUnit();
};