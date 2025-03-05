#pragma once
#include <string>
#include "ast.h"
#include "scanner.h"

class Paser {
private:
    scanner lex;  

    // 解析声明语句
    AstNode* parserExpr();
    AstNode* parserPrimaryExpr();

public:
    explicit Paser(const std::string& filename):lex(filename){}
    AstNode* parserPrag();
};