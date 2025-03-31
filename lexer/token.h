#pragma once
#include <string>
#include <unordered_map>
#include "source.h"

// token 类型
enum class TokenKind {
    // keyword 
    Alignof,
    Auto,
    Break,
    Case,
    Char,
    Const,
    Continue,
    Default,
    Do,
    Double,
    Else,
    Enum,
    Extern,
    Float,
    For,
    Semantics,
    Goto,
    If,
    Inline,
    Int,
    Long,
    Register,
    Restrict,
    Return,
    Short,
    Signed,
    Sizeof,
    Static,
    Struct,
    Switch,
    Typedef,
    Union,
    Unsigned,
    Void,
    Volatile,
    While,
    T_Alignas,
    T_Atomic,
    T_Bool,
    T_Complex,
    T_Generic,
    T_Imaginary,
    T_Noreturn,
    T_Static_assert,
    T_Thread_local,

    // identifier
    identifier,

    // constant
    Numeric_Constant_,
    Int_Constant_,
    Float_Constant,
    /* Enum_Constant_, */

    // string-literal
    String_Constant_,
    Character_Constant_,

    // punctuator
    //  {} [] () . ->
    LCurly_Brackets_,
    RCurly_Brackets_,
    LSquare_Brackets_,
    RSquare_Brackets_,
    LParent_,
    RParent_,
    Dot_,
    Arrow_,

    // ++ -- & * + - ~ !
    Increment_,
    Decrement_,
    BitWise_AND_,
    Multiplication_,
    Addition_,
    Subtraction_,
    BitWise_NOT_,
    Logical_NOT_,

    // / % << >> < > <= >= == != ^ && ||
    Division_,
    Modulus_,
    LShift_,
    RShift_,
    Less_,
    Greater_,
    Less_Equal_,
    Greater_Equal_,
    Equality_,
    Inequality_,
    BitWise_XOR_,
    Logical_AND_,
    Logical_OR_,

    // ？ ： ;  ...
    Conditional_, 
    Colon_,
    Semicolon_,  
    Ellipsis_,   

    //  = *= /= %= += -=  <<= >>= &= ^= |=
    Assign_,
    Mult_Assign_,
    Div_Assign_,
    Mod_Assign_,
    Add_Assign_,
    Sub_Assign_,
    LShift_Assign_,
    RShift_Assign_,
    BitWise_AND__Assign_,
    BitWise_XOR_Assign_,
    BitWise_OR_Assign_,

    // , # ##
    Comma_,
    Pound_, 
    Pasting_,

    // <: :> <% %> %: %:%: 
    LAlternative_Curly_Brackets_,
    RAlternative_Curly_Brackets_,
    LAlternative_Square_Brackets_,
    RAlternative_Square_Brackets_,
    Alternative_Pound_,
    Alternative_TokenPasting_,

    // 注释Token
    Comment_,

    // 错误Token
    Error_,

    // 无效token
    Invalid_,

    // \n eof
    NewLine,
    EOF_,

    // test
    Test_,
    Test_1,
};

const std::unordered_map<std::string, TokenKind> KeyWordMap = {
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

class Token {
private:
    TokenKind kind;
    std::string value;
    SourceLocation location;

public:
    explicit Token(TokenKind type, const std::string& value, SourceLocation loc)
    : kind(type), value(value), location(loc){}

    explicit Token(TokenKind type, SourceLocation loc)
    : kind(type), value(""), location(loc){}

    Token(const Token& tk)
    : kind(tk.kind), value(tk.value), location(tk.location){}

    Token& operator=(const Token& tk) {
        if (this != &tk) {
            kind = tk.kind;
            value = tk.value;
            location = tk.location;
        }
        return *this;
    }

    friend bool operator==(const Token& t1, const Token& t2) {
        return t1.getKind() == t2.getKind();
    }

    TokenKind getKind() const {
        return kind;
    }
    std::string getValue() const {
        return value;
    }
    SourceLocation getLocation() const {
        return location;
    }

    bool is(TokenKind kd) const {
        return kd == kind;
    }

    bool isEOF() const {
        return is(TokenKind::EOF_);
    }
};
