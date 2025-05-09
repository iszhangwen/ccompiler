#pragma once
#include <string>
#include <deque>
#include "token.h"
#include "source.h"


// 扫描器从SourceBuffer读取字符，返回Token
class scanner {
private:
    SourceBuffer buf;
    std::deque<Token> tokenQue;

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

    Token scanIdentifier();
    Token scanNumberLiteral();
    Token scanStringLiteral();
    Token scanCharLiter();

    Token scanLineComment();
    Token scanFullComment();

    Token scan();
    inline Token makeToken(TokenKind);
public:
    explicit scanner(std::string filename):buf(filename){}

    // 辅助函数
    Token next();
    Token peek(size_t);

    // 日志记录函数
    void log(const std::string& val);
};
