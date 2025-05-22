#include "scanner.h"
#include <iostream>

bool scanner::isLetter(char ch)
{
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
        return true;
    }
    return false;
}

bool scanner::isDecimal(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return true;
    }
    return false;
}

bool scanner::isHexacimal(char ch)
{
    if ((ch >= '0' && ch <= '9') || (ch <= 'f' && ch >= 'a') || (ch <= 'F' && ch >= 'A')) {
        return true;
    }
    return false;
}

bool scanner::isOctal(char ch) {
    if (ch >= '0' && ch <= '7') {
        return true;
    }
    return false;
}

bool scanner::isBinary(char ch) {
    if (ch == '0' || ch == '1') {
        return true;
    }
    return false;
}

bool scanner::isProcessMumberic(char ch)
{
    if (isLetter(ch) || isDecimal(ch) || ch == '.') {
        return true;
    }
    return false;
}

bool scanner::isEscapeChar(char ch)
{
    switch (ch)
    {
        case '\'':
        case '\"':
        case '\?':
        case '\\':
        case '\a':
        case '\b':
        case '\f':
        case '\n':
        case '\r':
        case '\t':
        case '\v':
            return true;
        default:
            return false;
    }
}

bool scanner::isUCN(char ch)
{
    if (ch == '\\' && (buf.peek() == 'u' || buf.peek() == 'U')) {
        return true;
    }
    return false;
}

// UCN 字符串范围
bool scanner::skipUCN(int len)
{
    bool flag = true;
    for (int i = 0; i < len; i++) {
        if (!isHexacimal(buf.nextch())) {
            flag = false;
        }
    }
    return flag;
}

bool scanner::skipEscape(char ch)
{
    switch (ch)
    {
    case '\'': case '\"': case '\?':
    case '\\': case 'a' : case 'b' : 
    case 'f' : case 'n' : case 'r' :
    case 't' : case 'v' :
        buf.nextch();
        return true;
    case 'u':
        return skipUCN(4);
    case 'U':
        return skipUCN(8);
    default:
        return false;
    }
}

Token scanner::scanIdentifier()
{
    char ch = buf.nextch();
    while (isLetter(ch) || isDecimal(ch) || isUCN(ch)) {

        if (isUCN(ch)) {
            skipEscape(ch);
        }
        ch = buf.nextch();
    }
    
    // 关键字查找
    auto iter = Token::KeyWordMap.find(buf.seg());
    TokenKind kind = (iter != Token::KeyWordMap.end()) ? iter->second : TokenKind::identifier;
    return makeToken(kind);   
}


Token scanner::scanNumberLiteral()
{
    char ch = buf.nextch();
    // 若下一个字符是预处理数字，则移动指针。
    while (isProcessMumberic(ch)) {
        ch = buf.nextch();
    }

    if (ch == 'e' || ch == 'E') {
        if (buf.match('+') || buf.match('-') || isProcessMumberic(buf.peek())) {
            return scanNumberLiteral();
        }
         return makeToken(TokenKind::Error_);
    }
    return makeToken(TokenKind::Numeric_Constant_);
}

Token scanner::scanStringLiteral()
{
    std::cout << "scanStringLiteral: " << (int)buf.curch() << "  " << (int)buf.peek() <<  " " << buf.segline() << "\n";
    do {
        if (buf.match('\"')) {
            buf.nextch();
            return makeToken(TokenKind::String_Constant_);
        }
        else if (buf.match('\n')) {
            return makeToken(TokenKind::Error_);
        }
        else if (buf.match(EOF)) {
            return makeToken(TokenKind::Error_);
        }
        else {
            buf.nextch();
        }
    } while (true);
    std::cout << "------------------4--\n";
    return makeToken(TokenKind::Error_);
}

Token scanner::scanCharLiter()
{
    char ch = buf.nextch();
    switch (ch)
    {
    case '\'':
        buf.nextch();
        return makeToken(TokenKind::Error_);
    case '\\':
        if (buf.match('\'')) {
            buf.nextch();
            return makeToken(TokenKind::Error_);
        } else {
            skipEscape(ch);
            if (buf.match('\'')) {
                buf.nextch();
                return makeToken(TokenKind::Character_Constant_);
            } else {
                buf.nextch();
                return makeToken(TokenKind::Error_);
            }
        }
    case '\n':
        buf.nextch();
        return makeToken(TokenKind::Error_);
    default:
        if (buf.match('\'')) {
            buf.nextch();
            return makeToken(TokenKind::Character_Constant_);
        } else {
            return makeToken(TokenKind::Error_);
        }
    }
}

Token scanner::scanLineComment()
{
    char ch =  buf.nextch();
    while (ch != '\n' && ch != EOF) {
        ch = buf.nextch();
    }
    return makeToken(TokenKind::Comment_);
}

Token scanner::scanFullComment()
{
    char ch = buf.nextch();
    while (ch != EOF) {
        if (ch == '*' && buf.match('/')) {
            buf.nextch();
            return makeToken(TokenKind::Comment_);
        }
        ch = buf.nextch();
    }
    return makeToken(TokenKind::Error_);
}

Token scanner::makeToken(TokenKind kind)
{
    return Token::newObj(kind, buf.loc(), buf.seg());
}

Token scanner::scan()
{
    // 跳过空白行
    while (isspace(buf.curch())) {
        buf.nextch();
    }

    buf.mark();
    char ch = buf.curch();
    switch (ch)
    {
    case 'a'...'z': 
    case 'A'...'Z': 
    case '_':    
        return scanIdentifier();

    case '0'...'9':
        return scanNumberLiteral();

    case '\"':
        return scanStringLiteral();

    case '\'':
        return scanCharLiter();

    case '/':
        if (buf.match('/')) {
            return scanLineComment();
        } 
        else if (buf.match('*')) {
            return scanFullComment();
        } 
        else if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::Div_Assign_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::Division_);
        }
    
    case '\\':
        if (isUCN(ch)) {
            return scanIdentifier();
        } 
        else {
            return makeToken(TokenKind::Error_);
        }

    case '{':
        buf.nextch();
        return makeToken(TokenKind::LCurly_Brackets_);

    case '}':
        buf.nextch();
        return makeToken(TokenKind::RCurly_Brackets_);

    case '[':
        buf.nextch();
        return makeToken(TokenKind::LSquare_Brackets_);

    case ']':
        buf.nextch();
        return makeToken(TokenKind::RSquare_Brackets_);

    case '(':
        buf.nextch();
        return makeToken(TokenKind::LParent_);

    case ')':
        buf.nextch();
        return makeToken(TokenKind::RParent_);

    case '.':
        if (isDecimal(buf.peek())) {
            return scanNumberLiteral();
        } 
        else if (buf.match('.')) {
            if (buf.match('.')) {
                buf.nextch();
                return makeToken(TokenKind::Ellipsis_);
            } 
            else {
                return makeToken(TokenKind::Error_);
            }
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::Dot_);
        }

    case '-':
        if (buf.match('>')) {
            buf.nextch();
            return makeToken(TokenKind::Arrow_);
        } 
        else if (buf.match('-')) {
            buf.nextch();
            return makeToken(TokenKind::Decrement_);
        } 
        else if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::Sub_Assign_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::Subtraction_);
        }
            
    case '+':
        if (buf.match('+')) {
            buf.nextch();
            return makeToken(TokenKind::Increment_);
        } 
        else if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::Add_Assign_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::Addition_);
        }

    case '&':
        if (buf.match('&')) {
            buf.nextch();            
            return makeToken(TokenKind::Logical_AND_);
        } 
        else if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::BitWise_AND__Assign_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::BitWise_AND_);
        }

    case '*':
        if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::Mult_Assign_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::Multiplication_);
        }

    case '~':
        buf.nextch();
        return makeToken(TokenKind::RParent_);

    case '!':
        if (buf.match('=')) {
            buf.nextch();            
            return makeToken(TokenKind::Inequality_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::RParent_);
        }

    case '%':
        if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::Mod_Assign_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::Modulus_);
        }

    case '<':
        if (buf.match('<')) {
            if (buf.match('=')) {
                buf.nextch();
                return makeToken(TokenKind::LShift_Assign_);
            } 
            else {
                buf.nextch();
                return makeToken(TokenKind::LShift_);
            } 
        } 
        else if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::Less_Equal_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::Less_);
        }

    case '>':
        if (buf.match('>')) {
            if (buf.match('=')) {
                buf.nextch();
                return makeToken(TokenKind::RShift_Assign_);
            } 
            else {
                buf.nextch();
                return makeToken(TokenKind::RShift_);
            }
        } 
        else if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::Greater_Equal_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::Greater_);
        }
    
    case '=':
        if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::Equality_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::Assign_);
        }
    
    case '^':
        if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::BitWise_XOR_Assign_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::BitWise_XOR_);
        }

    case '|':
        if (buf.match('|')) {
            buf.nextch();
            return makeToken(TokenKind::Logical_OR_);
        } 
        else if (buf.match('=')) {
            buf.nextch();
            return makeToken(TokenKind::BitWise_OR_Assign_);
        } 
        else {
            buf.nextch();
            return makeToken(TokenKind::Error_);
        }
    
    case '?':
        buf.nextch();
        return makeToken(TokenKind::Conditional_);

    case ':':
        buf.nextch();
        return makeToken(TokenKind::Colon_);

    case ';':
        buf.nextch();
        return makeToken(TokenKind::Semicolon_);

    case ',':
        buf.nextch();
        return makeToken(TokenKind::Comma_);

    case EOF:
        buf.nextch();
        return makeToken(TokenKind::EOF_);

    default:
        buf.nextch();
        return makeToken(TokenKind::Error_);
    }
}

void scanner::error(Token tk)
{
    fprintf(stderr, "%s: hanppen error\n", tk.loc_.filename.c_str());
}

TokenSequence scanner::tokenize()
{
    TokenSequence seq;
    Token tk = scan();
    while (!tk.isEOF())
    {
        seq.push_back(tk);
        tk = scan();
    }
    std::cout << "Token size: " << seq.size() << "\n";
    return seq;
}
