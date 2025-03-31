#pragma once
#include <string>
#include "ast.h"
#include "scanner.h"

class Parse {
private:
    scanner lex;
    Token tok;
    Token nextToken();
    // 中间辅助函数 
    bool match(TokenKind); 
    bool expect(TokenKind);

    // 错误相关函数
    void reportError();
    // ASTNode节点相关
    

    // 解析声明
    bool parserExternalDeclaration(AstNode* node);
    bool parserDeclaration(AstNode* node);
    bool parserFunctionDeclaration(AstNode* node);

    // 解析函数声明
    bool parserDeclarationSpecifiers(AstNode* node);
    bool parserDeclarationList(AstNode* node);

    //-----------------------------------------------------------------------
    // 6.5.1
    bool parserPrimaryExpr(AstNode* node);

    // 6.5.2


    //-----------------------------------------------------------------------
    // 解析语句相关
    Stmt parseStmt();

    // 6.8.1 lable语句
    LableStmt parseLableStmt();
    CaseStmt parseCaseStmt();
    DefaultStmt parseDefaultStmt();
    // 6.8.2 label语句
    CompoundStmt parseCompoundStmt();
    // 6.8.4 selection语句
    IfStmt parseIfStmt();
    SwitchStmt parseSwitchStmt();
    // 6.8.5 Iteration语句
    WhileStmt parseWhileStmt();
    DoStmt parseDoStmt();
    ForStmt parseForStmt();
    // 6.6.6 jump 语句
    GotoStmt parseGotoStmt();
    ContinueStmt parseContinueStmt();
    BreakStmt parseBreakStmt();
    ReturnStmt parseReturnStmt();
    //-----------------------------------------------------------------------

public:
    explicit Parse(const std::string& filename);
    bool parserTranslationUnit(AstNode* node);
};