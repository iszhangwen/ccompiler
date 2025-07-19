#include "token.h"
#include "scanner.h"
#include <sstream>
#include "../base/error.h"

Token *Token::newObj(TokenKind type, SourceLocation loc, const std::string& value)
{
    return new Token(type, loc, value);
}

Token *Token::newObj(TokenKind type, SourceLocation loc)
{
    return Token::newObj(type, loc, "");
}

Token *Token::newObj(TokenKind type)
{
    return Token::newObj(TokenKind::EOF_, SourceLocation());
}

const std::unordered_map<std::string, TokenKind> Token::KeyWordMap = {
    {"alignof",       TokenKind::Alignof,	   },
    {"auto",          TokenKind::Auto,	       },
    {"break",         TokenKind::Break,	       },
    {"case",          TokenKind::Case,	       },
    {"char",          TokenKind::Char,	       },
    {"const",         TokenKind::Const,	       },
    {"continue",      TokenKind::Continue,     },
    {"default",       TokenKind::Default,	   },
    {"do",            TokenKind::Do,	       },
    {"double",        TokenKind::Double,	   },
    {"else",          TokenKind::Else,	       },
    {"enum",          TokenKind::Enum,	       },
    {"extern",        TokenKind::Extern,	   },
    {"float",         TokenKind::Float,	       },
    {"for",           TokenKind::For,	       },
    {"Semantics",     TokenKind::Semantics,    },
    {"goto",          TokenKind::Goto,	       },
    {"if",            TokenKind::If,	       },
    {"inline",        TokenKind::Inline,	   },
    {"int",           TokenKind::Int,	       },
    {"long",          TokenKind::Long,	       },
    {"register",      TokenKind::Register,     },
    {"restrict",      TokenKind::Restrict,     },
    {"return",        TokenKind::Return,	   },
    {"short",         TokenKind::Short,	       },
    {"signed",        TokenKind::Signed,	   },
    {"sizeof",        TokenKind::Sizeof,	   },
    {"static",        TokenKind::Static,	   },
    {"struct",        TokenKind::Struct,	   },
    {"switch",        TokenKind::Switch,	   },
    {"typedef",       TokenKind::Typedef,	   },
    {"union",         TokenKind::Union,	       },
    {"unsigned",      TokenKind::Unsigned,     },
    {"void",          TokenKind::Void,	       },
    {"volatile",      TokenKind::Volatile,     },
    {"while",         TokenKind::While,	       },
    {"_Alignas",      TokenKind::T_Alignas,    },
    {"_Atomic",       TokenKind::T_Atomic,	   },
    {"_Bool",         TokenKind::T_Bool,	   },
    {"_Complex",      TokenKind::T_Complex,    },
    {"_Generic",      TokenKind::T_Generic,    },
    {"_Imaginary",    TokenKind::T_Imaginary,  },
    {"_Noreturn",     TokenKind::T_Noreturn,   },
    {"_Static_assert",TokenKind::T_Static_assert,	},
    {"_Thread_local", TokenKind::T_Thread_local,	}
};

const std::unordered_map<TokenKind, std::string> Token::TokenKindMap = 
{
    #define X_MACROS(a, b) {TokenKind::a, b},
    MACROS_TABLE
    #undef X_MACROS
};

bool Token::isEOF() const
{
    return kind_ == TokenKind::EOF_;
}

bool Token::isTypeSpecifier() const
{
    if (kind_ == TokenKind::Const
    || kind_ == TokenKind::Volatile
    || kind_ == TokenKind::Restrict) {
        return true;
    }
    return false;
}

void TokenSequence::dump() const {
    std::cout << "------------------------------\n";
    for (int i = 0; i < seq_.size(); i++)
    {
        std::cout << "kind: " << (int)seq_[i]->kind_ << " value: " << seq_[i]->value_ <<"\n";
    }
    std::cout << "------------------------------\n";
}

Token *TokenSequence::peek(size_t n) const
{
    if (pos_ + n >= size())
    {
        return Token::newObj(TokenKind::EOF_);
    }
    return seq_[pos_ + n];
}

Token *TokenSequence::next()
{
    pos_++;
    return cur();
}

Token *TokenSequence::cur() const
{
    return peek(0);
}

bool TokenSequence::match(TokenKind tk)
{
    if (peek()->kind_ == tk)
    {
        next();
        return true;
    }
    return false;
}

void TokenSequence::expect(TokenKind tk)
{
    if (peek()->kind_ == tk)
    {
        next();
        return;
    }
    std::stringstream ss;
    ss << "Expect " << Token::TokenKindMap.at(tk) << " but " << Token::TokenKindMap.at(peek()->kind_);
    throw CCError(ss.str());
}