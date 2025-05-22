#include "parse.h"

Parse::Parse(const std::string& filename)
: lex(filename) {}

bool Parse::match(TokenKind kind)
{
    return false;
}

bool Parse::expect(TokenKind kind)
{
    return false;
}

bool Parse::peek(size_t num, TokenKind kind)
{

    return true;
}

Token Parse::nextToken()
{
    return Token::newObj(TokenKind::EOF_, SourceLocation());
}

bool Parse::parserTranslationUnit(AstNode* node)
{
    return parserExternalDeclaration(node);
}

bool Parse::parserExternalDeclaration(AstNode* node)
{
    return true;
}

bool Parse::parserDeclaration(AstNode* node)
{
    return false;
}

bool Parse::parserFunctionDeclaration(AstNode* node)
{
    return false;
}

/*-----------------------------parse expression---------------------------------*/
bool Parse::parserPrimaryExpr(AstNode* node)
{
    return true;
}

/*
ExprStmt Parse::parseExpr()
{
    return ExprStmt();
}
*/

/*6.8
 statement:
    labeled-statement
    compound-statement
    expression-statement
    selection-statement
    iteration-statement
    jump-statement
依次解析stmt
*//*
Stmt Parse::parseStmt()
{
    TokenKind kind = tok.getKind();
    switch (kind)
    {
    case TokenKind::Case:
        return parseCaseStmt();
    case TokenKind::Default:
        return parseDefaultStmt();
    case TokenKind::RCurly_Brackets_:
        return parseCompoundStmt();
    case TokenKind::Semicolon_:
        return Stmt();
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
        if (peek(1, TokenKind::Colon_)) {
            return parseLabelStmt();
        }
    default:
        return parseExpr();
    }
}

LabelStmt Parse::parseLabelStmt()
{
    return LabelStmt();
}

CaseStmt Parse::parseCaseStmt()
{
    return CaseStmt();
}

DefaultStmt Parse::parseDefaultStmt()
{
    return DefaultStmt();
}

// 6.8.2 label语句
CompoundStmt Parse::parseCompoundStmt()
{
    return CompoundStmt();
}

// 6.8.4 selection语句
IfStmt Parse::parseIfStmt()
{
    return IfStmt();
}

SwitchStmt Parse::parseSwitchStmt()
{
    return SwitchStmt();
}

// 6.8.5 Iteration语句
WhileStmt Parse::parseWhileStmt()
{
    return WhileStmt();
}

DoStmt Parse::parseDoStmt()
{
    return DoStmt();
}

ForStmt Parse::parseForStmt()
{
    return ForStmt();
}

// 6.6.6 jump 语句
GotoStmt Parse::parseGotoStmt()
{
    return GotoStmt();
}

ContinueStmt Parse::parseContinueStmt()
{
    return ContinueStmt();
}

BreakStmt Parse::parseBreakStmt()
{
    return BreakStmt();
}

ReturnStmt Parse::parseReturnStmt()
{
    return ReturnStmt();
}
*/