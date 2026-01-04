#pragma once
#include <string>
#include <deque>
#include "token.h"
#include "source.h"
#include "error.h"

// 扫描器从SourceBuffer读取字符，返回Token
class scanner {
private:
    Source *buf_;
    static bool isLetter(char);
    static bool isDecimal(char);
    static bool isHexacimal(char);
    static bool isOctal(char);
    static bool isBinary(char);
    static bool isProcessMumberic(char);
    static bool isEscapeChar(char);
    
    // 处理USN
    bool isUCN(char);
    bool skipUCN(int len);
    // 处理转义字符
    bool skipEscape(char);
    bool match(const Token& tk);

    Token *scanIdentifier();
    Token *scanNumberLiteral();
    Token *scanStringLiteral();
    Token *scanCharLiter();

    Token *scanLineComment();
    Token *scanFullComment();

    inline Token *makeToken(TokenKind);

    // 错误打印函数
    void error(const std::string& val);
    void error(SourceLocation loc, const std::string& val);

public:
    explicit scanner(Source* buf);
    // 核心扫描函数
    Token *scan();
    TokenSequence tokenize();
};
