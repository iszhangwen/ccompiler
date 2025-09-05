#include "preprocessor.h"
#include <scanner.h>
#include <iostream>
#include <unordered_map>
#include <vector>   

void Preprocessor::PreprocessorError(SourceLocation loc, const std::string& val)
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

// Implementations of Preprocessor methods
void Preprocessor::handleMacroDefinition() 
{
    // 实现宏定义处理逻辑
}

void Preprocessor::handleMacroUnDefinition() 
{
    // 取消宏定义
}

void Preprocessor::handleConditionDirective() 
{
    // 实现条件编译指令处理逻辑
}
// 实现包含文件指令处理逻辑
void Preprocessor::handleIncludeDirective() 
{
    std::string fileName;
    // 1.#include "test.h"
    if (seq.peek()->kind_ == TokenKind::String_Constant_) 
    {
        fileName = seq.next()->value_;
        seq.skipNewLine();
    } 
    // 2.#include <test.h>
    else if (seq.peek()->kind_ == TokenKind::Less_) 
    {
        seq.next();
        while (seq.peek()->kind_ != TokenKind::Greater_)
        {
            if (seq.peek()->isLineBegin_ || seq.peek()->isEOF())
            {
                //PreprocessorError(seq)
            }
            fileName.append(seq.next()->value_);
        }
        seq.skipNewLine();
    }
    // 3. #define "test.h" TEST
    // #include TEST
    else if (seq.peek()->kind_ == TokenKind::Identifier)
    {

    }
    // 利用解析出来的Seq获取Token
    Scanner sc(Source(fileName));
    TokenSequence seq = sc.tokenize();
    return preprocess(seq);
}

void Preprocessor::handleComment() 
{
    // 实现注释处理逻辑
}

void Preprocessor::handleStringAndCharacterLiterals() 
{
    // 实现字符串和字符字面量处理逻辑
}

void Preprocessor::handleExpandMacro() 
{
    // 展开宏
}

void Preprocessor::process(TokenSequence& is, TokenSequence& os)
{
    while (!is.next()->isEOF())
    {
        std::vector<Token> res;
        // 处理预处理指令
        switch (is.cur()->kind_)
        {
        case TokenKind::T_Include:
            res = handleIncludeDirective(seq);
            break;
        case TokenKind::T_Define:
            res = handleMacroDefinition(seq);
            break;
        case TokenKind::T_Undefine:
            res = handleMacroUnDefinition(seq);
            break;

        case TokenKind::T_Ifdef:
        case TokenKind::T_Ifndef:
        case TokenKind::T_Elif:
        case TokenKind::T_Else:
        case TokenKind::T_Endif:
            res = handleConditionDirective(seq);
            break;

        case TokenKind::T_Line:
            break;
        case TokenKind::T_Error:
            break;
        case TokenKind::T_Pragma:
            break;
        case TokenKind::Pound_:
            break;
        case TokenKind::Pasting_:
            break;
        default:
            res = handleExpandMacro(seq);
            break;
        }
        // 插入新的Token
        for (int i = 0; i < res.size(); i++){
            newSeq.push_back(res[i]);
        }
    }
}

// 预处理问题
TokenSequence Preprocessor::preprocess(Source* buf)
{
    TokenSequence os;
    auto is = Scanner::tokenize(buf);
    process(is, os);
}