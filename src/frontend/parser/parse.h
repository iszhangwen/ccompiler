#pragma once
#include <string>
#include <decl.h>
#include <stmt.h>
#include <expr.h>
#include <type.h>
#include <scanner.h>
#include <sema.h>

class Parser;

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

    // 语法错误处理策略，遇到错误时中止编译程序
    void sytaxError(const std::string& val);
    void sytaxError(Token *tk, const std::string& val);
    // 语义错误处理策略
    void semaError(const std::string& val);
    void semaError(Token *tk, const std::string& val);

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
    Decl* parseInitDeclarator(Declarator);
    //Declarator parseInitDeclarator(QualType qt, int sc, int fs);
    // 6.7.1 storage-class-specifier
    void parseStorageClassSpec(StorageClass val, int* sc);
    // 6.7.2 type-specifier
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
    void parseFunctionSpec(FuncSpecifier val, int* fs);
    // 6.7.5 declarator
    void parseDeclarator(Declarator&);
    // 复杂声明时，由于最后解析函数或数组声明，需要对变量类型做修正。
    QualType modifyBaseType(QualType, QualType, QualType);
    Expr* parseArrayLen();
    QualType parseFuncOrArrayDeclarator(QualType qt);
    QualType parsePointer(QualType);
    void parseParameterTypeList();
    std::vector<ParmVarDecl*> parseParameterList();
    void parseParameterDeclaration();
    void parseIdentifierList();
    // 6.7.6 type-name
    QualType parseTypeName();
    void parseAbstractDeclarator();
    void parseDirectAbstractDeclarator();
    // 6.7.7 typedef-name
    void parseTypedefName();
    // 6.7.8 initializer
    Expr* parseInitializer();
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
    void dump(ASTVisitor* av) {
        if (av) {
            unit_->accept(av);
        }
    }
};