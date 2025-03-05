#include "parse.h"

AstNode* Paser::parserPrag()
{
    return NULL;
}


AstNode* Paser::parserExpr()
{
    return NULL;
}

AstNode* Paser::parserPrimaryExpr()
{
    Token tk = lex.peek();
    switch (tk.getKind())
    {
    case TokenKind::identifier:
        break;
    
    case TokenKind::Numeric_Constant_:
        break;

    case TokenKind::String_Constant_:
        break;

    case TokenKind::LParent_:
        break;

    case TokenKind::T_Generic:
        break;
    default:
        break;
    }
}