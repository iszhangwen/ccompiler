#include "token.h"
#include "scanner.h"
#include <sstream>
#include "../base/error.h"

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

const std::unordered_map<std::string, TokenKind> Token::PreprocessKeyWordMap
{
    {"if", TokenKind::T_If},
    {"ifdef", TokenKind::T_Ifdef},
    {"ifndef", TokenKind::T_Ifndef},

    {"elif", TokenKind::T_Elif},
    {"else", TokenKind::T_Else},
    {"endif", TokenKind::T_Endif},

    {"include", TokenKind::T_Include},
    {"define", TokenKind::T_Define},
    {"undef", TokenKind::T_Undefine},

    {"line", TokenKind::T_Line},
    {"error", TokenKind::T_Error},
    {"pragma", TokenKind::T_Pragma}
};

bool Token::isEOF() const
{
    return kind_ == TokenKind::EOF_;
}

Token* Token::getEOFToken()
{
    static Token tk = Token(TokenKind::EOF_);
    return &tk;
}

bool Token::isTypeQualifier() const
{
    if (kind_ == TokenKind::Const
    || kind_ == TokenKind::Volatile
    || kind_ == TokenKind::Restrict) {
        return true;
    }
    return false;
}

bool Token::isComment() const
{
    if (kind_ == TokenKind::Comment_)
    {
        return true;
    }
    return false;
}

bool Token::isProcessDirective() const
{
    if ((kind_ == TokenKind::T_If)
    || (kind_ == TokenKind::T_Ifdef)
    || (kind_ == TokenKind::T_Ifndef)
    || (kind_ == TokenKind::T_Elif)
    || (kind_ == TokenKind::T_Else)
    || (kind_ == TokenKind::T_Endif)
    || (kind_ == TokenKind::T_Include)
    || (kind_ == TokenKind::T_Define)
    || (kind_ == TokenKind::T_Undefine)
    || (kind_ == TokenKind::T_Line)
    || (kind_ == TokenKind::T_Error)
    || (kind_ == TokenKind::T_Pragma)
    || (kind_ == TokenKind::Pound_)
    || (kind_ == TokenKind::Pasting_)) {
        return true;
    }
    return false;
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

void TokenSequence::dump()  
{
    std::cout << "------------------------------\n";
    for (int i = 0; i < seq_.size(); i++)
    {
        std::cout << "kind: " << (int)seq_[i].kind_ << " value: " << seq_[i].value_ <<"\n";
    }
    std::cout << "------------------------------\n";
}

Token *TokenSequence::peek(size_t n) 
{
    if (pos_ + n >= size())
    {
        return Token::getEOFToken();
    }
    return &seq_[pos_ + n];
}

Token *TokenSequence::next()
{
    pos_++;
    return cur();
}

Token *TokenSequence::cur() 
{
    return peek(0);
}

void TokenSequence::reset()
{
    if (pos_ >= 0) {
        pos_--;
    }
}

void TokenSequence::skipNewLine()
{
    while (!cur()->isLineBegin_)
    {
        next();
    }
}

void TokenSequence::push_back(Token tk)
{
    // 检测是否为预处理器指令
    if (size() <= 0 || cur()->kind_ != TokenKind::Pound_) {
        seq_.push_back(tk);
        return;
    }
    // 预处理指令 ##
    if (tk.kind_ == TokenKind::Pound_)
    {
        cur()->value_ += tk.value_; ;
        cur()->kind_ = TokenKind::Pasting_;
        return;
    }
    // 处理预处理指令 #Idenfier
    else if (tk.kind_ == TokenKind::Identifier) {
        // 检查是否为预处理器指令
        auto iter = Token::PreprocessKeyWordMap.find(tk.value_);
        if (iter != Token::PreprocessKeyWordMap.end()) {
            cur()->value_ = tk.value_;
            cur()->kind_ = iter->second;
        }
        // 如果不是预处理器指令，则将其作为普通宏指令 
        else {
            cur()->value_ = tk.value_;
            cur()->kind_ = TokenKind::T_Macro;
        }
        return;
    }
    // 其他类型的Token直接添加
    seq_.push_back(tk);
    return;
}