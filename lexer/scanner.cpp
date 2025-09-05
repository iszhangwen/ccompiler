#include "scanner.h"
#include <sstream>

bool Scanner::isLetter(char ch)
{
    if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_') {
        return true;
    }
    return false;
}

bool Scanner::isDecimal(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return true;
    }
    return false;
}

bool Scanner::isHexacimal(char ch)
{
    if ((ch >= '0' && ch <= '9') || (ch <= 'f' && ch >= 'a') || (ch <= 'F' && ch >= 'A')) {
        return true;
    }
    return false;
}

bool Scanner::isOctal(char ch) {
    if (ch >= '0' && ch <= '7') {
        return true;
    }
    return false;
}

bool Scanner::isBinary(char ch) {
    if (ch == '0' || ch == '1') {
        return true;
    }
    return false;
}

bool Scanner::isProcessMumberic(char ch)
{
    if (isLetter(ch) || isDecimal(ch) || ch == '.') {
        return true;
    }
    return false;
}

bool Scanner::isEscapeChar(char ch)
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

bool Scanner::isUCN(char ch)
{
    if (ch == '\\' && (buf_->peek() == 'u' || buf_->peek() == 'U')) {
        return true;
    }
    return false;
}

// UCN 字符串范围
bool Scanner::skipUCN(int len)
{
    bool flag = true;
    for (int i = 0; i < len; i++) {
        if (!isHexacimal(buf_->nextch())) {
            flag = false;
        }
    }
    return flag;
}

bool Scanner::skipEscape(char ch)
{
    switch (ch)
    {
    case '\'': case '\"': case '\?':
    case '\\': case 'a' : case 'b' : 
    case 'f' : case 'n' : case 'r' :
    case 't' : case 'v' :
        buf_->nextch();
        return true;
    case 'u':
        return skipUCN(4);
    case 'U':
        return skipUCN(8);
    default:
        return false;
    }
}

Token Scanner::scanIdentifier()
{
    char ch = buf_->nextch();
    while (isLetter(ch) || isDecimal(ch) || isUCN(ch)) {

        if (isUCN(ch)) {
            skipEscape(ch);
        }
        ch = buf_->nextch();
    }
    
    // 关键字查找
    auto iter = Token::KeyWordMap.find(buf_->seg());
    TokenKind kind = (iter != Token::KeyWordMap.end()) ? iter->second : TokenKind::Identifier;
    return makeToken(kind);   
}


Token Scanner::scanNumberLiteral()
{
    char ch = buf_->nextch();
    // 若下一个字符是预处理数字，则移动指针。
    while (isProcessMumberic(ch)) {
        ch = buf_->nextch();
    }

    if (ch == 'e' || ch == 'E') {
        if (buf_->match('+') || buf_->match('-') || isProcessMumberic(buf_->peek())) {
            return scanNumberLiteral();
        }
         return makeToken(TokenKind::Error_);
    }
    return makeToken(TokenKind::Numeric_Constant_);
}

Token Scanner::scanStringLiteral()
{
    do {
        if (buf_->match('\"')) {
            buf_->nextch();
            return makeToken(TokenKind::String_Constant_);
        }
        else if (buf_->match('\n')) {
            error("unExpect stringLiteral!");
        }
        else if (buf_->match(EOF)) {
            throw ("unExpect stringLiteral!");
        }
        else {
            buf_->nextch();
        }
    } while (true);
    error("unExpect stringLiteral!");
}

Token Scanner::scanCharLiter()
{
    char ch = buf_->nextch();
    switch (ch)
    {
    case '\'':
        buf_->nextch();
        return makeToken(TokenKind::Error_);
    case '\\':
        if (buf_->match('\'')) {
            buf_->nextch();
            return makeToken(TokenKind::Error_);
        } else {
            skipEscape(ch);
            if (buf_->match('\'')) {
                buf_->nextch();
                return makeToken(TokenKind::Character_Constant_);
            } else {
                buf_->nextch();
                return makeToken(TokenKind::Error_);
            }
        }
    case '\n':
        buf_->nextch();
        return makeToken(TokenKind::Error_);
    default:
        if (buf_->match('\'')) {
            buf_->nextch();
            return makeToken(TokenKind::Character_Constant_);
        } else {
            return makeToken(TokenKind::Error_);
        }
    }
}

Token Scanner::scanLineComment()
{
    char ch =  buf_->nextch();
    while (ch != '\n' && ch != EOF) {
        ch = buf_->nextch();
    }
    return makeToken(TokenKind::Comment_);
}

Token Scanner::scanFullComment()
{
    char ch = buf_->nextch();
    while (ch != EOF) {
        if (ch == '*' && buf_->match('/')) {
            buf_->nextch();
            return makeToken(TokenKind::Comment_);
        }
        ch = buf_->nextch();
    }
    return makeToken(TokenKind::Error_);
}

void Scanner::error(const std::string& val)
{
   error(buf_->loc(), val);
}

void Scanner::error(SourceLocation loc, const std::string& val)
{
    #define RED "\033[31m"
    #define CANCEL "\033[0m"
    std::stringstream ss;
    ss << loc.filename 
        << ":" 
        << loc.line 
        << ":" 
        << loc.column 
        << ": "
        << RED 
        << "error: " 
        << CANCEL
        << val 
        << std::endl
        << buf_->segline(loc)
        << std::string(loc.column, ' ') 
        << "^ "  
        << RED 
        << std::string(buf_->segline(loc).size() - loc.column - 2, '~') 
        << CANCEL 
        << std::endl;
    #undef RED
    #undef CANCEL
    throw CCError(ss.str());
}

Token Scanner::makeToken(TokenKind kind)
{
    return Token(kind, buf_->islinehead(), buf_->loc(), buf_->seg());
}

Scanner::Scanner(Source* buf)
: buf_(buf)
{
    if (buf_ == nullptr)
    {
        error("SourceBuffer is nullptr");
    }
}

Token Scanner::scan()
{
    // 跳过空白行
    while (isspace(buf_->curch())) {
        buf_->nextch();
    }

    buf_->mark();
    char ch = buf_->curch();
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
        if (buf_->match('/')) {
            return scanLineComment();
        } 
        else if (buf_->match('*')) {
            return scanFullComment();
        } 
        else if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::Div_Assign_);
        } 
        else {
            buf_->nextch();
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
        buf_->nextch();
        return makeToken(TokenKind::LCurly_Brackets_);

    case '}':
        buf_->nextch();
        return makeToken(TokenKind::RCurly_Brackets_);

    case '[':
        buf_->nextch();
        return makeToken(TokenKind::LSquare_Brackets_);

    case ']':
        buf_->nextch();
        return makeToken(TokenKind::RSquare_Brackets_);

    case '(':
        buf_->nextch();
        return makeToken(TokenKind::LParent_);

    case ')':
        buf_->nextch();
        return makeToken(TokenKind::RParent_);

    case '.':
        if (isDecimal(buf_->peek())) {
            return scanNumberLiteral();
        } 
        else if (buf_->match('.')) {
            if (buf_->match('.')) {
                buf_->nextch();
                return makeToken(TokenKind::Ellipsis_);
            } 
            else {
                return makeToken(TokenKind::Error_);
            }
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::Dot_);
        }

    case '-':
        if (buf_->match('>')) {
            buf_->nextch();
            return makeToken(TokenKind::Arrow_);
        } 
        else if (buf_->match('-')) {
            buf_->nextch();
            return makeToken(TokenKind::Decrement_);
        } 
        else if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::Sub_Assign_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::Subtraction_);
        }
            
    case '+':
        if (buf_->match('+')) {
            buf_->nextch();
            return makeToken(TokenKind::Increment_);
        } 
        else if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::Add_Assign_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::Addition_);
        }

    case '&':
        if (buf_->match('&')) {
            buf_->nextch();            
            return makeToken(TokenKind::Logical_AND_);
        } 
        else if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::BitWise_AND_Assign_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::BitWise_AND_);
        }

    case '*':
        if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::Mult_Assign_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::Multiplication_);
        }

    case '~':
        buf_->nextch();
        return makeToken(TokenKind::RParent_);

    case '!':
        if (buf_->match('=')) {
            buf_->nextch();            
            return makeToken(TokenKind::Inequality_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::RParent_);
        }

    case '%':
        if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::Mod_Assign_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::Modulus_);
        }

    case '<':
        if (buf_->match('<')) {
            if (buf_->match('=')) {
                buf_->nextch();
                return makeToken(TokenKind::LShift_Assign_);
            } 
            else {
                buf_->nextch();
                return makeToken(TokenKind::LShift_);
            } 
        } 
        else if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::Less_Equal_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::Less_);
        }

    case '>':
        if (buf_->match('>')) {
            if (buf_->match('=')) {
                buf_->nextch();
                return makeToken(TokenKind::RShift_Assign_);
            } 
            else {
                buf_->nextch();
                return makeToken(TokenKind::RShift_);
            }
        } 
        else if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::Greater_Equal_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::Greater_);
        }
    
    case '=':
        if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::Equality_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::Assign_);
        }
    
    case '^':
        if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::BitWise_XOR_Assign_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::BitWise_XOR_);
        }

    case '|':
        if (buf_->match('|')) {
            buf_->nextch();
            return makeToken(TokenKind::Logical_OR_);
        } 
        else if (buf_->match('=')) {
            buf_->nextch();
            return makeToken(TokenKind::BitWise_OR_Assign_);
        } 
        else {
            buf_->nextch();
            return makeToken(TokenKind::Error_);
        }
    
    case '?':
        buf_->nextch();
        return makeToken(TokenKind::Conditional_);

    case ':':
        buf_->nextch();
        return makeToken(TokenKind::Colon_);

    case ';':
        buf_->nextch();
        return makeToken(TokenKind::Semicolon_);

    case ',':
        buf_->nextch();
        return makeToken(TokenKind::Comma_);

    case '#':
        buf_->nextch();
        return makeToken(TokenKind::Pound_);

    case EOF:
        buf_->nextch();
        return makeToken(TokenKind::EOF_);

    default:
        buf_->nextch();
        return makeToken(TokenKind::Error_);
    }
}

TokenSequence Scanner::tokenize()
{
    TokenSequence seq;
    Token tk = scan();
    while (!tk.isEOF())
    {
        seq.push_back(tk);
        tk = scan();
    }
    seq.push_back(tk);
    return seq;
}

TokenSequence Scanner::tokenize(Source* buf)
{
    Scanner sc(buf);
    return sc.tokenize();
}


