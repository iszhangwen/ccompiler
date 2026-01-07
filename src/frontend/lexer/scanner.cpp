#include "scanner.h"
#include <sstream>
#include "ccerror.h"

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
    if (ch == '\\' && (m_source->peek() == 'u' || m_source->peek() == 'U')) {
        return true;
    }
    return false;
}

// UCN 字符串范围
bool scanner::skipUCN(int len)
{
    bool flag = true;
    for (int i = 0; i < len; i++) {
        if (!isHexacimal(m_source->nextch())) {
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
        m_source->nextch();
        return true;
    case 'u':
        return skipUCN(4);
    case 'U':
        return skipUCN(8);
    default:
        return false;
    }
}

Token *scanner::scanIdentifier()
{
    char ch = m_source->nextch();
    while (isLetter(ch) || isDecimal(ch) || isUCN(ch)) {

        if (isUCN(ch)) {
            skipEscape(ch);
        }
        ch = m_source->nextch();
    }
    
    // 关键字查找
    auto iter = Token::KeyWordMap.find(m_source->seg());
    TokenKind kind = (iter != Token::KeyWordMap.end()) ? iter->second : TokenKind::Identifier;
    return makeToken(kind);   
}


Token *scanner::scanNumberLiteral()
{
    char ch = m_source->nextch();
    // 若下一个字符是预处理数字，则移动指针。
    while (isProcessMumberic(ch)) {
        ch = m_source->nextch();
    }

    if (ch == 'e' || ch == 'E') {
        if (m_source->match('+') || m_source->match('-') || isProcessMumberic(m_source->peek())) {
            return scanNumberLiteral();
        }
         return makeToken(TokenKind::Error_);
    }
    return makeToken(TokenKind::Numeric_Constant_);
}

Token *scanner::scanStringLiteral()
{
    do {
        if (m_source->match('\"')) {
            m_source->nextch();
            return makeToken(TokenKind::String_Constant_);
        }
        else if (m_source->match('\n')) {
            error("unExpect stringLiteral!");
        }
        else if (m_source->match(EOF)) {
            throw ("unExpect stringLiteral!");
        }
        else {
            m_source->nextch();
        }
    } while (true);
    error("unExpect stringLiteral!");
}

Token *scanner::scanCharLiter()
{
    char ch = m_source->nextch();
    switch (ch)
    {
    case '\'':
        m_source->nextch();
        return makeToken(TokenKind::Error_);
    case '\\':
        if (m_source->match('\'')) {
            m_source->nextch();
            return makeToken(TokenKind::Error_);
        } else {
            skipEscape(ch);
            if (m_source->match('\'')) {
                m_source->nextch();
                return makeToken(TokenKind::Character_Constant_);
            } else {
                m_source->nextch();
                return makeToken(TokenKind::Error_);
            }
        }
    case '\n':
        m_source->nextch();
        return makeToken(TokenKind::Error_);
    default:
        if (m_source->match('\'')) {
            m_source->nextch();
            return makeToken(TokenKind::Character_Constant_);
        } else {
            return makeToken(TokenKind::Error_);
        }
    }
}

Token *scanner::scanLineComment()
{
    char ch =  m_source->nextch();
    while (ch != '\n' && ch != EOF) {
        ch = m_source->nextch();
    }
    return makeToken(TokenKind::Comment_);
}

Token *scanner::scanFullComment()
{
    char ch = m_source->nextch();
    while (ch != EOF) {
        if (ch == '*' && m_source->match('/')) {
            m_source->nextch();
            return makeToken(TokenKind::Comment_);
        }
        ch = m_source->nextch();
    }
    return makeToken(TokenKind::Error_);
}

void scanner::error(const std::string& val)
{
   error(m_source->loc(), val);
}

void scanner::error(SourceLocation loc, const std::string& val)
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
        << m_source->segline(loc)
        << std::string(loc.column, ' ') 
        << "^ "  
        << RED 
        << std::string(m_source->segline(loc).size() - loc.column - 2, '~') 
        << CANCEL 
        << std::endl;
    #undef RED
    #undef CANCEL
    throw CCError(ss.str());
}

Token *scanner::makeToken(TokenKind kind)
{
    return Token::newObj(kind, m_source->loc(), m_source->seg());
}

scanner::scanner(std::shared_ptr<Source> buf)
: m_source(buf)
{
    if (m_source == nullptr)
    {
        error("SourceBuffer is nullptr");
    }
}

Token *scanner::scan()
{
    // 跳过空白行
    while (isspace(m_source->curch())) {
        m_source->nextch();
    }

    m_source->mark();
    char ch = m_source->curch();
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
        if (m_source->match('/')) {
            return scanLineComment();
        } 
        else if (m_source->match('*')) {
            return scanFullComment();
        } 
        else if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::Div_Assign_);
        } 
        else {
            m_source->nextch();
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
        m_source->nextch();
        return makeToken(TokenKind::LCurly_Brackets_);

    case '}':
        m_source->nextch();
        return makeToken(TokenKind::RCurly_Brackets_);

    case '[':
        m_source->nextch();
        return makeToken(TokenKind::LSquare_Brackets_);

    case ']':
        m_source->nextch();
        return makeToken(TokenKind::RSquare_Brackets_);

    case '(':
        m_source->nextch();
        return makeToken(TokenKind::LParent_);

    case ')':
        m_source->nextch();
        return makeToken(TokenKind::RParent_);

    case '.':
        if (isDecimal(m_source->peek())) {
            return scanNumberLiteral();
        } 
        else if (m_source->match('.')) {
            if (m_source->match('.')) {
                m_source->nextch();
                return makeToken(TokenKind::Ellipsis_);
            } 
            else {
                return makeToken(TokenKind::Error_);
            }
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::Dot_);
        }

    case '-':
        if (m_source->match('>')) {
            m_source->nextch();
            return makeToken(TokenKind::Arrow_);
        } 
        else if (m_source->match('-')) {
            m_source->nextch();
            return makeToken(TokenKind::Decrement_);
        } 
        else if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::Sub_Assign_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::Subtraction_);
        }
            
    case '+':
        if (m_source->match('+')) {
            m_source->nextch();
            return makeToken(TokenKind::Increment_);
        } 
        else if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::Add_Assign_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::Addition_);
        }

    case '&':
        if (m_source->match('&')) {
            m_source->nextch();            
            return makeToken(TokenKind::Logical_AND_);
        } 
        else if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::BitWise_AND_Assign_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::BitWise_AND_);
        }

    case '*':
        if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::Mult_Assign_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::Multiplication_);
        }

    case '~':
        m_source->nextch();
        return makeToken(TokenKind::RParent_);

    case '!':
        if (m_source->match('=')) {
            m_source->nextch();            
            return makeToken(TokenKind::Inequality_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::RParent_);
        }

    case '%':
        if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::Mod_Assign_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::Modulus_);
        }

    case '<':
        if (m_source->match('<')) {
            if (m_source->match('=')) {
                m_source->nextch();
                return makeToken(TokenKind::LShift_Assign_);
            } 
            else {
                m_source->nextch();
                return makeToken(TokenKind::LShift_);
            } 
        } 
        else if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::Less_Equal_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::Less_);
        }

    case '>':
        if (m_source->match('>')) {
            if (m_source->match('=')) {
                m_source->nextch();
                return makeToken(TokenKind::RShift_Assign_);
            } 
            else {
                m_source->nextch();
                return makeToken(TokenKind::RShift_);
            }
        } 
        else if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::Greater_Equal_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::Greater_);
        }
    
    case '=':
        if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::Equality_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::Assign_);
        }
    
    case '^':
        if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::BitWise_XOR_Assign_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::BitWise_XOR_);
        }

    case '|':
        if (m_source->match('|')) {
            m_source->nextch();
            return makeToken(TokenKind::Logical_OR_);
        } 
        else if (m_source->match('=')) {
            m_source->nextch();
            return makeToken(TokenKind::BitWise_OR_Assign_);
        } 
        else {
            m_source->nextch();
            return makeToken(TokenKind::Error_);
        }
    
    case '?':
        m_source->nextch();
        return makeToken(TokenKind::Conditional_);

    case ':':
        m_source->nextch();
        return makeToken(TokenKind::Colon_);

    case ';':
        m_source->nextch();
        return makeToken(TokenKind::Semicolon_);

    case ',':
        m_source->nextch();
        return makeToken(TokenKind::Comma_);

    case EOF:
        m_source->nextch();
        return makeToken(TokenKind::EOF_);

    default:
        m_source->nextch();
        return makeToken(TokenKind::Error_);
    }
}

TokenSequence scanner::tokenize()
{
    TokenSequence seq;
    Token *tk = scan();
    while (!tk->isEOF())
    {
        seq.push_back(tk);
        tk = scan();
    }
    seq.push_back(tk);
    return seq;
}
