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
    ScopeManager(Parser*, Scope::ScopeType st);
    ~ScopeManager();
};

class Parser {
private:
    using std::shared_ptr<Expr> = std::shared_ptr<Expr>;
    friend ScopeManager;
    std::shared_ptr<Source> m_source;
    std::shared_ptr<TokenSequence> m_tokenSeq;
    std::shared_ptr<TranslationUnitDecl> m_unit;
    std::shared_ptr<SemaAnalyzer> m_sema;
    std::shared_ptr<SymbolTableContext> m_systable;

    // 语法错误处理策略，遇到错误时中止编译程序
    void sytaxError(const std::string& val);
    void sytaxError(Token *tk, const std::string& val);
    // 语义错误处理策略
    void semaError(const std::string& val);
    void semaError(Token *tk, const std::string& val);

    /*-----------------------------Expressions-----------------------------------------*/
    std::shared_ptr<Expr> parsePrimaryExpr();
    std::shared_ptr<Expr> parseGenericSelection();
    std::shared_ptr<Expr> parseGenericAssociation();
    std::shared_ptr<Expr> parsePostfixExpr();
    std::vector<std::shared_ptr<Expr>> parseArgListExpr();
    std::shared_ptr<Expr> parseUnaryExpr();
    std::shared_ptr<Expr> parseCastExpr();
    std::shared_ptr<Expr> parseMultiExpr();
    std::shared_ptr<Expr> parseAddExpr();
    std::shared_ptr<Expr> parseShiftExpr();
    std::shared_ptr<Expr> parseRelationalExpr();
    std::shared_ptr<Expr> parseEqualExpr();
    std::shared_ptr<Expr> parseBitANDExpr();
    std::shared_ptr<Expr> parseBitXORExpr();
    std::shared_ptr<Expr> parseBitORExpr();
    std::shared_ptr<Expr> parseLogicalANDExpr();
    std::shared_ptr<Expr> parseLogicalORExpr();
    std::shared_ptr<Expr> parseConditionalExpr();
    std::shared_ptr<Expr> parseAssignExpr();
    std::shared_ptr<Expr> parseExpr();
    std::shared_ptr<Expr> parseConstansExpr();
    // 该表达式不是文法符号，是为了简化分析歧义
    std::shared_ptr<Expr> parseParenExpr();
    /*-------------------------------Declarations--------------------------------------*/
    // 6.7 declaration
    DeclGroup parseDeclaration();
    Declarator parseDeclarationSpec();
    std::shared_ptr<DeclaratorDecl> parseInitDeclarator(Declarator);
    //Declarator parseInitDeclarator(QualType qt, int sc, int fs);
    // 6.7.1 storage-class-specifier
    void parseStorageClassSpec(StorageClass val, int* sc);
    // 6.7.2 type-specifier
    std::shared_ptr<RecordType> parseStructOrUnionSpec(bool isStruct);
    QualType parseStructDeclarationList(Symbol*);
    QualType parseSpecQualList();
    DeclGroup parseStructDeclaratorList(QualType qt, Decl* parent);
    std::shared_ptr<Decl> parseStructDeclarator(QualType qt);
    std::shared_ptr<EnumType> parseEnumSpec();
    QualType parseEnumeratorList(Symbol*, Type*);
    std::shared_ptr<EnumConstantDecl> parseEnumerator(QualType qt);
    // 6.7.3 type-qualifier
    int parseTypeQualList();
    // 6.7.4 function-specifier
    void parseFunctionSpec(FuncSpecifier val, int* fs);
    // 6.7.5 declarator
    void parseDeclarator(Declarator&);
    // 复杂声明时，由于最后解析函数或数组声明，需要对变量类型做修正。
    QualType modifyBaseType(QualType, QualType, QualType);
    std::shared_ptr<Expr> parseArrayLen();
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
    std::shared_ptr<Expr> parseInitializer();
    void parseInitializerList();
    void parseDesignation();
    void parseDesignator();
    /*--------------------------------Statements---------------------------------------*/
    std::shared_ptr<Stmt> parseStmt();
    std::shared_ptr<Stmt> parseLabeledStmt();
    std::shared_ptr<CompoundStmt> parseCompoundStmt();
    std::shared_ptr<ExprStmt> parseExprStmt();
    std::shared_ptr<Stmt> parseSelectionStmt();
    std::shared_ptr<Stmt> parseIterationStmt();
    std::shared_ptr<Stmt> parseJumpStmt();
    /*-------------------------------External definitions-------------------------------*/
    Decl* parseFunctionDefinitionBody(Decl*);

public:
    explicit Parser(const std::string& filename);
    virtual ~Parser();
    // 解析根节点
    void parseTranslationUnit();
    void dump(ASTVisitor* av) {
        if (av) {
            m_unit->accept(av);
        }
    }
};
