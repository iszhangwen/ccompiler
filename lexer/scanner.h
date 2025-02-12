#pragma once
#include <string>
#include "token.h"
#include "source.h"


// 扫描器从SourceBuffer读取字符，返回Token
class scanner {
private:
    SourceBuffer buffer;
    std::vector<Token> tokenBuffer;

    static bool isLetter(char);
    static bool isDecimal(char);
    static bool isHexacimal(char);
    static bool isOctal(char);
    static bool isBinary(char);
    static bool isProcessMumberic(char);

    Token scanIdentifier();
    Token scanNumberLiteral();
    Token scanStringLiteral();
    Token scanCharLiter();
    Token scanComment();

    Token scan();
    inline Token makeToken(TokenKind);
public:
    explicit scanner(std::string filename):
    buffer(filename){}

    Token nextToken();
    Token peek();
};
