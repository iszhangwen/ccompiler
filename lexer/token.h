#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include "source.h"

// token 类型
#define MACROS_TABLE \
    X_MACROS(Alignof, "alignof")\
    X_MACROS(Auto, "auto")\
    X_MACROS(Break, "break")\
    X_MACROS(Case, "case")\
    X_MACROS(Char, "char")\
    X_MACROS(Const, "const")\
    X_MACROS(Continue, "continue")\
    X_MACROS(Default, "default")\
    X_MACROS(Do, "do")\
    X_MACROS(Double, "double")\
    X_MACROS(Else, "else")\
    X_MACROS(Enum, "enum")\
    X_MACROS(Extern, "extern")\
    X_MACROS(Float, "float")\
    X_MACROS(For, "for")\
    X_MACROS(Semantics, "semantics")\
    X_MACROS(Goto, "goto")\
    X_MACROS(If, "if")\
    X_MACROS(Inline, "inline")\
    X_MACROS(Int, "int")\
    X_MACROS(Long, "long")\
    X_MACROS(Register, "register")\
    X_MACROS(Restrict, "restrict")\
    X_MACROS(Return, "return")\
    X_MACROS(Short, "short")\
    X_MACROS(Signed, "signed")\
    X_MACROS(Sizeof, "sizeof")\
    X_MACROS(Static, "static")\
    X_MACROS(Struct, "struct")\
    X_MACROS(Switch, "switch")\
    X_MACROS(Typedef, "typedef")\
    X_MACROS(Union, "union")\
    X_MACROS(Unsigned, "unsigned")\
    X_MACROS(Void, "void")\
    X_MACROS(Volatile, "volatile")\
    X_MACROS(While, "while")\
    X_MACROS(T_Alignas, "_alignas")\
    X_MACROS(T_Atomic, "_atomic")\
    X_MACROS(T_Bool, "_Bool")\
    X_MACROS(T_Complex, "_Complex")\
    X_MACROS(T_Generic, "_Generic")\
    X_MACROS(T_Imaginary, "_Imainary")\
    X_MACROS(T_Noreturn, "_Noreturn")\
    X_MACROS(T_Static_assert, "_Static-assert")\
    X_MACROS(T_Thread_local, "_Thread_local")\
    X_MACROS(Identifier, "identifier")\
    X_MACROS(Numeric_Constant_, "Numeric_Constant_")\
    X_MACROS(Float_Constant, "Float_Constant")\
    X_MACROS(String_Constant_, "String_Constant_")\
    X_MACROS(Character_Constant_, "Character_Constant_")\
    X_MACROS(LCurly_Brackets_, "{")\
    X_MACROS(RCurly_Brackets_, "}")\
    X_MACROS(LSquare_Brackets_, "[")\
    X_MACROS(RSquare_Brackets_, "]")\
    X_MACROS(LParent_, "(")\
    X_MACROS(RParent_, ")")\
    X_MACROS(Dot_, ".")\
    X_MACROS(Arrow_, "->")\
    X_MACROS(Increment_, "++")\
    X_MACROS(Decrement_, "--")\
    X_MACROS(Multiplication_, "*")\
    X_MACROS(Addition_, "+")\
    X_MACROS(Subtraction_, "-")\
    X_MACROS(Division_, "/")\
    X_MACROS(Modulus_, "%")\
    X_MACROS(LShift_, "<<")\
    X_MACROS(RShift_, ">>")\
    X_MACROS(Less_, "<")\
    X_MACROS(Greater_, ">")\
    X_MACROS(Less_Equal_, "<=")\
    X_MACROS(Greater_Equal_, ">=")\
    X_MACROS(Equality_, "==")\
    X_MACROS(Inequality_, "!=")\
    X_MACROS(BitWise_AND_, "&")\
    X_MACROS(BitWise_XOR_, "^")\
    X_MACROS(BitWise_OR_, "|")\
    X_MACROS(BitWise_NOT_, "~")\
    X_MACROS(Logical_AND_, "&&")\
    X_MACROS(Logical_OR_, "||")\
    X_MACROS(Logical_NOT_, "!")\
    X_MACROS(Conditional_, "\?")\
    X_MACROS(Colon_, ":")\
    X_MACROS(Semicolon_, ";")\
    X_MACROS(Ellipsis_, "...")\
    X_MACROS(Assign_, "=")\
    X_MACROS(Mult_Assign_, "*=")\
    X_MACROS(Div_Assign_, "/=")\
    X_MACROS(Mod_Assign_, "%=")\
    X_MACROS(Add_Assign_, "+=")\
    X_MACROS(Sub_Assign_, "-=")\
    X_MACROS(LShift_Assign_, "<<=")\
    X_MACROS(RShift_Assign_, ">>=")\
    X_MACROS(BitWise_AND_Assign_, "&=")\
    X_MACROS(BitWise_XOR_Assign_, "^=")\
    X_MACROS(BitWise_OR_Assign_, "|=")\
    X_MACROS(Comma_, ",")\
    X_MACROS(Pound_, "#")\
    X_MACROS(Pasting_, "##")\
    X_MACROS(LAlternative_Curly_Brackets_, "<:")\
    X_MACROS(RAlternative_Curly_Brackets_, ":>")\
    X_MACROS(LAlternative_Square_Brackets_, "<%")\
    X_MACROS(RAlternative_Square_Brackets_, "%>")\
    X_MACROS(Alternative_Pound_, "%:")\
    X_MACROS(Alternative_TokenPasting_, "%:%:")\
    X_MACROS(Comment_, "comment")\
    X_MACROS(Error_, "error")\
    X_MACROS(Invalid_, "invalid")\
    X_MACROS(NewLine, "newline")\
    X_MACROS(EOF_, "EOF")\

enum class TokenKind
{
    #define X_MACROS(a, b) a,
    MACROS_TABLE
    #undef X_MACROS
};

class Token {
public:
    // token kind
    const TokenKind kind_;

    // token value
    const std::string value_;

    // loc
    const SourceLocation loc_;

    // creat a new Token
    static Token *newObj(TokenKind type);
    static Token *newObj(TokenKind type, SourceLocation loc);
    static Token *newObj(TokenKind type, SourceLocation loc, const std::string& value);

    // static member 
    static const std::unordered_map<std::string, TokenKind> KeyWordMap;
    static const std::unordered_map<TokenKind, std::string> TokenKindMap;

    // func
    bool isEOF() const;

    // typespecifier
    bool isTypeSpecifier() const;

private:
    Token(TokenKind type, SourceLocation loc, const std::string& value)
    : kind_(type), loc_(loc), value_(value){}

    Token(TokenKind type, SourceLocation loc)
    : Token(type, loc, ""){}
};

class TokenSequence 
{
private:
    int pos_{-1};
    std::vector<Token*> seq_;

public:
    void push_back(Token* tk) {seq_.push_back(tk);}

    // 辅助函数
    size_t size() const {return seq_.size();}
    void dump() const;
    // 提供给语法分析器的辅助函数
    Token* next();
    Token* cur() const;
    Token* peek(size_t n = 1) const; 
    bool match(TokenKind);
    void expect(TokenKind);
    bool test(TokenKind);
};