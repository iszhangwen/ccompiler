#pragma once
#include <string>
#include <ast/decl.h>
#include <ast/stmt.h>
#include <ast/expr.h>
#include <ast/type.h>
#include <scanner.h>
#include <scope.h>
#include <sema.h>

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
    static void accept(TSState& curState, TokenKind cond, int& ts);
};

class Parser {
private:
    using DeclList = std::vector<Decl*>;
    Scope *curScope_;
    Source *buf_;
    TokenSequence *seq_;
    TranslationUnitDecl* unit_;
    sema* sema_;

    // token串访问函数
    bool Expect(TokenKind);

    /*
    错误处理策略，遇到错误是中止编译程序
    */
    void error(const std::string& val);
    void error(Token *tk, const std::string& val);

        // 作用域栈函数
    void enterScope(Scope::ScopeType);
    void exitScope();

    // 解析声明
    std::vector<Decl*> parseExternalDeclaration();
    bool parseDeclaration();

    // 解析声明
    QualType parseDeclarationSpecifiers(int *StorageClass, int *funSpec);
    DeclaratorDecl* parseDeclarator(Declarator&);
    void parseInitializer();
    void parseInitDeclarator();


    //-----------------------------------------------------------------------
    // 6.5.1
    Expr *parsePrimaryExpr();

    // 6.5.2

    Expr *parseExpr();
    //-----------------------------------------------------------------------
    // statement
    Stmt *parseStmt();

    // 6.8.1 lable语句
    LabelStmt *parseLabelStmt();
    CaseStmt *parseCaseStmt();
    DefaultStmt *parseDefaultStmt();
    // 6.8.2 label语句
    CompoundStmt *parseCompoundStmt();
    // 6.8.4 selection语句
    IfStmt *parseIfStmt();
    SwitchStmt *parseSwitchStmt();
    // 6.8.5 Iteration语句
    WhileStmt *parseWhileStmt();
    DoStmt *parseDoStmt();
    ForStmt *parseForStmt();
    // 6.6.6 jump 语句
    GotoStmt *parseGotoStmt();
    ContinueStmt *parseContinueStmt();
    BreakStmt *parseBreakStmt();
    ReturnStmt *parseReturnStmt();
    //-----------------------------------------------------------------------

public:
    explicit Parser(const std::string& filename);
    virtual ~Parser();
    // 解析根节点
    void parseTranslationUnit();

    // 调试打印
    void dumpAST();
    void dumpTokens();
};