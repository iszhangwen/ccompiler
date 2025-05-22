#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
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
    EOF_
};


class Token {
public:
    // token kind
    TokenKind kind_;

    // token value
    std::string value_;

    // loc
    SourceLocation loc_;

    // creat a new Token
    static Token newObj(TokenKind type, SourceLocation loc);
    static Token newObj(TokenKind type, SourceLocation loc, const std::string& value);

    // static member 
    static const std::unordered_map<std::string, TokenKind> KeyWordMap;

    // func
    bool isEOF() const;

private:
    Token(TokenKind type, SourceLocation loc, const std::string& value)
    : kind_(type), loc_(loc), value_(value){}

    Token(TokenKind type, SourceLocation loc)
    : Token(type, loc, ""){}
};

class TokenSequence 
{
private:
    int curPos_{-1};
    std::vector<Token> seq_;

public:
    void push_back(const Token& tk) {seq_.push_back(tk);}

    // 辅助函数
    size_t size() const {return seq_.size();}
    Token next() { return seq_[++curPos_];}

    void dump() {
        for (int i = 0; i < seq_.size(); i++)
        {
            std::cout << "kind: " << (int)seq_[i].kind_ << " value: " << seq_[i].value_ <<"\n";
        }
        std::cout << "------------------------------\n";
    }
};