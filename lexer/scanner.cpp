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

Token scanner::scanIdentifier()
{
    while (isLetter(buffer.peek()) || isDecimal(buffer.peek())) {
        buffer.nextch();
    }
    auto iter = KeyWordMap.find(buffer.ObtainSegment());
    TokenKind kind = (iter != KeyWordMap.end()) ? iter->second : TokenKind::identifier;
    return makeToken(kind);   
}

// 预处理阶段尽可能的放宽， 字面值合法性放到语法检查阶段进行检查
Token scanner::scanNumberLiteral()
{
    // 若下一个字符是预处理数字，则移动指针。
    char prevCh = buffer.peek();
    while (isProcessMumberic(buffer.peek())) {
        prevCh = buffer.nextch();
    }

    if (prevCh == 'e' || prevCh == 'E') {
        if (buffer.match('+')) {
            return scanNumberLiteral();
        } else if (buffer.match('-')) {
            return scanNumberLiteral();
        }
    }
    return makeToken(TokenKind::Numeric_Constant_);
}

Token scanner::scanStringLiteral()
{
    do {
        if (buffer.match('\"')) 
            return makeToken(TokenKind::String_Constant_);
        else if (buffer.match('\n'))
            return makeToken(TokenKind::Error_);
        else if (buffer.match(EOF)) 
            return makeToken(TokenKind::EOF_);
        else 
            buffer.nextch();
    } while (true);
    return makeToken(TokenKind::Error_);
}

Token scanner::scanCharLiter()
{
    if (buffer.peek(1) != '\'') {
        return makeToken(TokenKind::Error_);
    }
    return makeToken(TokenKind::Character_Constant_);
}

Token scanner::scanComment()
{
    // 形式的注释
    if (buffer.match('/')) {  
        char ch =  buffer.nextch();
        while (ch != '\n' && ch != EOF) {
            ch = buffer.nextch();
        }
        return makeToken(TokenKind::Comment_);
    } 
    /* 形式的注释 */
    else if (buffer.match('*')) {
        char ch = buffer.nextch();
        while (ch != '*' || !buffer.match('/')) {
            ch = buffer.nextch();
        }
        return makeToken(TokenKind::Comment_);
    }
    return makeToken(TokenKind::Error_);
}

Token scanner::makeToken(TokenKind kind)
{
    return Token(kind, buffer.ObtainSegment(), buffer.ObtainLocation());
}

Token scanner::scan()
{
    // 跳过空白行
    while (isspace(buffer.peek())) {
        buffer.nextch();
    }

    buffer.mark();
    char ch = buffer.nextch();
    switch (ch)
    {
    case 'a'...'z': case 'A'...'Z': case '_':    
        return scanIdentifier();
    case '0'...'9':
        return scanNumberLiteral();
    case '\"':
        return scanStringLiteral();
    case '\'':
        return scanCharLiter();
    case '/':
        if (buffer.peek() == '*' || buffer.peek() == '/') {
            return scanComment();
        } else if (buffer.match('=')) {
            return makeToken(TokenKind::Div_Assign_);
        } else {
            return makeToken(TokenKind::Division_);
        }

    case '{':
        return makeToken(TokenKind::LCurly_Brackets_);
    case '}':
        return makeToken(TokenKind::RCurly_Brackets_);
    case '[':
        return makeToken(TokenKind::LSquare_Brackets_);
    case ']':
        return makeToken(TokenKind::RSquare_Brackets_);
    case '(':
        return makeToken(TokenKind::LParent_);
    case ')':
        return makeToken(TokenKind::RParent_);
    case '.':
        if (isDecimal(buffer.peek())) {
            return scanNumberLiteral();
        } else if (buffer.match('.')) {
            if (buffer.match('.')) {
                return makeToken(TokenKind::Ellipsis_);
            } else {
                return makeToken(TokenKind::Error_);
            }
        } 
        else {
            return makeToken(TokenKind::Dot_);
        }

    case '-':
        if (buffer.match('>')) {
            return makeToken(TokenKind::Arrow_);
        } else if (buffer.match('-')) {
            return makeToken(TokenKind::Decrement_);
        } else if (buffer.match('=')) {
            return makeToken(TokenKind::Sub_Assign_);
        } else {
            return makeToken(TokenKind::Subtraction_);
        }
            
    case '+':
        if (buffer.match('+')) {
            return makeToken(TokenKind::Increment_);
        } else if (buffer.match('=')) {
            return makeToken(TokenKind::Add_Assign_);
        } else {
            return makeToken(TokenKind::Addition_);
        }

    case '&':
        if (buffer.match('&')) {
            return makeToken(TokenKind::Logical_AND_);
        } else if (buffer.match('=')) {
            return makeToken(TokenKind::BitWise_AND__Assign_);
        } else {
            return makeToken(TokenKind::BitWise_AND_);
        }

    case '*':
        if (buffer.match('=')) {
            return makeToken(TokenKind::Mult_Assign_);
        } else {
            return makeToken(TokenKind::Multiplication_);
        }

    case '~':
        return makeToken(TokenKind::RParent_);
    case '!':
        if (buffer.match('=')) {
            return makeToken(TokenKind::Inequality_);
        } else {
            return makeToken(TokenKind::RParent_);
        }

    case '%':
        if (buffer.match('=')) {
            return makeToken(TokenKind::Mod_Assign_);
        } else {
            return makeToken(TokenKind::Modulus_);
        }

    case '<':
        if (buffer.match('<')) {
            if (buffer.match('=')) {
                return makeToken(TokenKind::LShift_Assign_);
            } else {
                return makeToken(TokenKind::LShift_);
            } 
        } else if (buffer.match('=')) {
            return makeToken(TokenKind::Less_Equal_);
        } else {
            return makeToken(TokenKind::Less_);
        }

    case '>':
        if (buffer.match('>')) {
            if (buffer.match('=')) {
                return makeToken(TokenKind::RShift_Assign_);
            } else {
                return makeToken(TokenKind::RShift_);
            }
        } else if (buffer.match('=')) {
            return makeToken(TokenKind::Greater_Equal_);
        } else {
            return makeToken(TokenKind::Greater_);
        }
    
    case '=':
        if (buffer.match('=')) {
            return makeToken(TokenKind::Equality_);
        } else {
            return makeToken(TokenKind::Assign_);
        }
    
    case '^':
        if (buffer.match('=')) {
            return makeToken(TokenKind::BitWise_XOR_Assign_);
        } else {
            return makeToken(TokenKind::BitWise_XOR_);
        }

    case '|':
        if (buffer.match('|')) {
            return makeToken(TokenKind::Logical_OR_);
        } else if (buffer.match('=')) {
            return makeToken(TokenKind::BitWise_OR_Assign_);
        } else {
            return makeToken(TokenKind::Error_);
        }
    
    case '?':
        return makeToken(TokenKind::Conditional_);
    case ':':
        return makeToken(TokenKind::Colon_);
    case ';':
        return makeToken(TokenKind::Semicolon_);
    case ',':
        return makeToken(TokenKind::Comma_);

    case EOF:
        return makeToken(TokenKind::EOF_);
    default:
        return makeToken(TokenKind::Error_);
    }
}

Token scanner::nextToken()
{
    return scan();
}

Token scanner::peek()
{
    return Token(TokenKind::Error_, buffer.ObtainLocation());
}