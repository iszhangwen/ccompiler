#pragma once
#include <string>
#include <scanner.h>
#include <ast/decl.h>
#include <ast/stmt.h>
#include <ast/expr.h>
#include <ast/type.h>

class Parser {
private:
    Source buf_;
    TokenSequence seq_;
    // 错误相关函数
    void reportError();
    
    // 解析根节点
    bool parserTranslationUnit(AstNode* node);

    // 解析声明
    bool parserExternalDeclaration(AstNode* node);
    bool parserDeclaration(AstNode* node);
    bool parserFunctionDeclaration(AstNode* node);

    // 解析函数声明
    bool parserDeclarationSpecifiers(AstNode* node);
    bool parserDeclarationList(AstNode* node);

    //-----------------------------------------------------------------------
    // 6.5.1
    Expr *parserPrimaryExpr(AstNode* node);

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
    virtual ~Parser() {}
    bool parse(AstNode* node);
};