#include "parse.h"

Parser::Parser(const std::string& filename)
: buf_(filename) {
    scanner sc(&buf_);
    seq_ = sc.tokenize();
}

bool Parser::parserTranslationUnit(AstNode* node)
{
    return parserExternalDeclaration(node);
}

bool Parser::parserExternalDeclaration(AstNode* node)
{
    return true;
}

bool Parser::parserDeclaration(AstNode* node)
{
    return false;
}

bool Parser::parserFunctionDeclaration(AstNode* node)
{
    return false;
}

/*-----------------------------parse expression---------------------------------*/
bool Parser::parserPrimaryExpr(AstNode* node)
{
    return true;
}


Expr *Parser::parseExpr()
{
    return nullptr;
}

/*6.8
 statement:
    labeled-statement
    compound-statement
    expression-statement
    selection-statement
    iteration-statement
    jump-statement
依次解析stmt*/
Stmt *Parser::parseStmt()
{
    switch (TokenKind::Case)
    {
    case TokenKind::Case:
        return parseCaseStmt();
    case TokenKind::Default:
        return parseDefaultStmt();
    case TokenKind::RCurly_Brackets_:
        return parseCompoundStmt();
    case TokenKind::Semicolon_:
        return nullptr;
    case TokenKind::If:
        return parseIfStmt();
    case TokenKind::Switch:
        return parseSwitchStmt();
    case TokenKind::While:
        return parseWhileStmt();
    case TokenKind::Do:
        return parseDoStmt();
    case TokenKind::For:
        return parseForStmt();
    case TokenKind::Goto:
        return parseGotoStmt();
    case TokenKind::Continue:
        return parseContinueStmt();
    case TokenKind::Break:
        return parseBreakStmt();
    case TokenKind::Return:
        return parseReturnStmt();
    case TokenKind::identifier:
        return parseLabelStmt();
    default:
        return parseExpr();
    }
}

LabelStmt *Parser::parseLabelStmt()
{
    return nullptr;
}

CaseStmt *Parser::parseCaseStmt()
{
    return nullptr;
}

DefaultStmt *Parser::parseDefaultStmt()
{
    return nullptr;
}

// 6.8.2 label语句
CompoundStmt *Parser::parseCompoundStmt()
{
    return nullptr;
}

// 6.8.4 selection语句
IfStmt *Parser::parseIfStmt()
{
    return nullptr;
}

SwitchStmt *Parser::parseSwitchStmt()
{
    return nullptr;
}

// 6.8.5 Iteration语句
WhileStmt *Parser::parseWhileStmt()
{
    return nullptr;
}

DoStmt *Parser::parseDoStmt()
{
    return nullptr;
}

ForStmt *Parser::parseForStmt()
{
    return nullptr;
}

// 6.6.6 jump 语句
GotoStmt *Parser::parseGotoStmt()
{
    return nullptr;
}

ContinueStmt *Parser::parseContinueStmt()
{
    return nullptr;
}

BreakStmt *Parser::parseBreakStmt()
{
    return nullptr;
}

ReturnStmt *Parser::parseReturnStmt()
{
    return nullptr;
}


bool Parser::parse(AstNode* node)
{
    return true;
}