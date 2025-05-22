#pragma once
#include <string>
#include <scanner.h>
#include <ast/decl.h>
#include <ast/stmt.h>
#include <ast/expr.h>
#include <ast/type.h>

class Parse {
private:
    scanner lex;
    Token nextToken();
    /*辅助函数 
    * match: 匹配当前token类型，若匹配成功则移动到下一个，否则返回false
    * expect: 期望下一个token匹配类型，匹配成功返回true,否则返回false
    * test: 测试当前第几个token类型
    */ 
    bool match(TokenKind); 
    bool expect(TokenKind);
    bool peek(size_t, TokenKind);

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
/*
    ExprStmt parseExpr();
    //-----------------------------------------------------------------------
    // statement
    Stmt parseStmt();

    // 6.8.1 lable语句
    LabelStmt parseLabelStmt();
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
*/
public:
    explicit Parse(const std::string& filename);
    bool parserTranslationUnit(AstNode* node);
};