#include "parse.h"
#include <sema.h>
#include <sstream>

void ParseTypeSpec::accept(TSState& curState, TokenKind cond, int& ts)
{
    switch (cond)
    {
        case TokenKind::Void:
        if (curState == TSState::START) {
            curState = TSState::FOUND_TYPE;
            ts |= TypeSpecifier::VOID;
        } else {
            curState = TSState::ERROR;
        }
        break;

        case TokenKind::Char:
        if (curState == TSState::START 
            || curState == TSState::FOUND_SIGNED_UNSIGNED) {
            curState = TSState::FOUND_TYPE;
            ts |= TypeSpecifier::CHAR;
        } else {
            curState = TSState::ERROR;
        }
        break;

        case TokenKind::Int:
        if (curState == TSState::START
            || curState == TSState::FOUND_SIGNED_UNSIGNED
            ||curState == TSState::FOUND_SHORT
            ||curState == TSState::FOUND_LONG
            ||curState == TSState::FOUND_LONG2) {
            curState = TSState::FOUND_TYPE;
            ts |= TypeSpecifier::INT;
        } else {
            curState = TSState::ERROR;
        }
        break;

        case TokenKind::Float:
        if (curState == TSState::START) {
            curState = TSState::FOUND_TYPE;
            ts |= TypeSpecifier::FLOAT;
        } else {
            curState = TSState::ERROR;
        }
        break;

        case TokenKind::Double:
        if (curState == TSState::START
            || curState == TSState::FOUND_LONG) {
            curState = TSState::FOUND_TYPE;
            ts |= TypeSpecifier::DOUBLE;
        } else {
            curState = TSState::ERROR;
        }
        break;

        case TokenKind::Signed:
        if (curState == TSState::START) {
            curState = TSState::FOUND_SIGNED_UNSIGNED;
            ts |= TypeSpecifier::SIGNED;
        } else {
            curState = TSState::ERROR;
        }
        break;

        case TokenKind::Unsigned:
        if (curState == TSState::START) {
            curState = TSState::FOUND_SIGNED_UNSIGNED;
            ts |= TypeSpecifier::UNSIGNED;
        } else {
            curState = TSState::ERROR;
        }
        break;

        case TokenKind::Short:
        if (curState == TSState::START) {
            curState = TSState::FOUND_SHORT;
            ts |= TypeSpecifier::SHORT;
        } else {
            curState = TSState::ERROR;
        }
        break;

        case TokenKind::Long:
        if (curState == TSState::START
            || curState == FOUND_SIGNED_UNSIGNED) {
            curState = TSState::FOUND_LONG;
            ts |= TypeSpecifier::LONG;
        } else if (curState == TSState::FOUND_LONG) {
            curState = FOUND_LONG2;
            ts |= TypeSpecifier::LONG;
        } else {
            curState = TSState::ERROR;
        }
        break;
    
        default:
        curState = TSState::ERROR;
        break;
    }
}

Parser::Parser(const std::string& filename)
{
    buf_ = new Source(filename);
    seq_ = new TokenSequence(scanner(buf_).tokenize());
}

Parser:: ~Parser()
{
    delete buf_;
    delete seq_;
}

void Parser::error(const std::string& val)
{
    error(seq_->cur(), val);
}

void Parser::error(Token *tk, const std::string& val)
{
    #define RED "\033[31m"
    #define CANCEL "\033[0m"
    std::stringstream ss;
    ss << tk->loc_.filename 
        << ":" 
        << tk->loc_.line 
        << ":" 
        << tk->loc_.column 
        << ": "
        << RED 
        << "error: " 
        << CANCEL
        << val 
        << std::endl
        << buf_->segline(tk->loc_)
        << std::string(tk->loc_.column, ' ') 
        << "^ "  
        << RED 
        << std::string(buf_->segline(tk->loc_).size() - tk->loc_.column - 2, '~') 
        << CANCEL 
        << std::endl;
    #undef RED
    #undef CANCEL
    throw CCError(ss.str());
}

void Parser::enterScope(Scope::ScopeType st)
{
    curScope_ = new Scope(st, curScope_);
}

void Parser::exitScope()
{
    curScope_ = curScope_->getParent();
}

void Parser::parseTranslationUnit()
{
    enterScope(Scope::FILE);
    while (!seq_->match(TokenKind::EOF_)) {
        if (seq_->match(TokenKind::Semicolon_)) {
            continue;
        }
        auto res = parseExternalDeclaration();
    }
    //sema_->onActTranslationUnit(curScope_);
    exitScope();
}

/*
declaration-specifiers:
    storage-class-specifier declaration-specifiersopt
    type-specifier declaration-specifiersopt
    type-qualifier declaration-specifiersopt
    function-specifier declaration-specifiersopt
*/
QualType Parser::parseDeclarationSpecifiers(int *storageClass, int *funSpec)
{
    int tq = 0, ts = 0;
    ParseTypeSpec::TSState tss = ParseTypeSpec::START;
    while (true)
    {
        Token *tk = seq_->next();
        // (6.7.1) storage class specifier
        switch (tk->kind_)
        {
            case TokenKind::Typedef:
                if (!storageClass) {
                    error(tk, "unexpect typedef specifier!");
                } else if (*storageClass != 0) {
                    error(tk, "duplicated typedef specifier!");
                } else {
                    *storageClass = StorageClass::TYPEDEF;
                }
                break;

            case TokenKind::Extern:
                if (!storageClass) {
                    error(tk, "unexpect typedef specifier!");
                } else if (*storageClass != 0) {
                    error(tk, "duplicated typedef specifier!");
                } else {
                    *storageClass = StorageClass::TYPEDEF;
                }
                break;

            case TokenKind::Static:
                if (!storageClass) {
                    error(tk, "unexpect Static specifier!");
                } else if (*storageClass != 0) {
                    error(tk, "duplicated Static specifier!");
                } else {
                    *storageClass = StorageClass::STATIC;
                }
                break;

            case TokenKind::Auto:
                if (!storageClass) {
                    error(tk, "unexpect auto specifier!");
                } else if (*storageClass != 0) {
                    error(tk, "duplicated auto specifier!");
                } else {
                    *storageClass = StorageClass::AUTO;
                }
                break;

            case TokenKind::Register:
                if (!storageClass) {
                    error(tk, "unexpect register specifier!");
                } else if (*storageClass != 0) {
                    error(tk, "duplicated register specifier!");
                } else {
                    *storageClass = StorageClass::REGISTER;
                }
                break;

            // (6.7.4) function-specifier
            case TokenKind::Inline:
                if (!funSpec) {
                    error(tk, "unexpect inline specifier!");
                } else if (*funSpec != 0) {
                    error(tk, "duplicated inline specifier!");
                } else {
                    *funSpec |= FuncSpecifier::INLINE;
                }
                break;

            // (6.7.2) type specifiers
            // 使用状态机解析TypeSpecifier
            case TokenKind::Void:
            case TokenKind::Char:
            case TokenKind::Int:
            case TokenKind::Float:
            case TokenKind::Double:
            case TokenKind::Signed:
            case TokenKind::Unsigned:
            case TokenKind::Short:
            case TokenKind::Long:
                ParseTypeSpec::accept(tss, tk->kind_, ts);
                if (tss == ParseTypeSpec::ERROR) {
                    error(tk, "unexpect " + std::string(tk->value_) + " !");
                }
                break;
            // (6.7.2) type specifiers
            // 忽略这两项，编译器暂时不支持
            case TokenKind::T_Bool:
            case TokenKind::T_Complex:
            break;

            // (6.7.2) struct-or-union-specifier
            case TokenKind::Struct:
            case TokenKind::Union:
            // (6.7.2) enum-specifier
            case TokenKind::Enum:

             //(6.7.3) type-qualifier:
            case TokenKind::Const:   tq |= TypeQualifier::CONST;break;
            case TokenKind::Volatile:tq |= TypeQualifier::VOLATILE;break;
            case TokenKind::Restrict:tq |= TypeQualifier::RESTRICT;break;

            // (6.7.7) typedef-name 判断当前是否已有其他方式
            case TokenKind::identifier:
            if (tss == ParseTypeSpec::START) {
                curScope_->lookup(nullptr, nullptr);
                tss = ParseTypeSpec::FOUND_TYPE;
            } else {
                QualType qty(nullptr);
                
                return QualType(nullptr);
            }
                
            default:
                error(tk, "unexpect " + std::string(tk->value_) + " !");
                break;
        }
    }
}

/*
declarator:
    pointeropt direct-declarator
direct-declarator:
    identifier
    ( declarator )
    direct-declarator [ type-qualifier-listopt assignment-expressionopt ]
    direct-declarator [static type-qualifier-listopt assignment-expression ]
    direct-declarator [ type-qualifier-list static assignment-expression ]
    direct-declarator [ type-qualifier-listopt *]
    direct-declarator ( parameter-type-list )
    direct-declarator ( identifier-listopt 
*/
DeclaratorDecl* Parser::parseDeclarator(Declarator&)
{
    return nullptr;
}

void Parser::parseInitDeclarator()
{

}

void Parser::parseInitializer()
{

}

/*
 (6.9) external-declaration:
    function-definition
    declaration
 (6.9.1) function-definition:
    declaration-specifiers declarator declaration-listopt[ignore] compound-statement
 (6.9.1) declaration-list:
    declaration
    declaration-list declaration
(6.7) declaration:
    declaration-specifiers init-declarator-listopt ;
 (6.7) declaration-specifiers:
    storage-class-specifier declaration-specifiersopt
    type-specifier declaration-specifiersopt
    type-qualifier declaration-specifiersopt
    function-specifier declaration-specifiersopt
 (6.7) init-declarator-list:
    init-declarator
    init-declarator-list , init-declarator
 (6.7) init-declarator:
    declarator
    declarator = initializer
*/
std::vector<Decl*> Parser::parseExternalDeclaration()
{
    /*
    external-declaration:
        declaration-specifiers declarator declaration-listopt[ignore] compound-statement
        declaration-specifiers declarator = initializer;
        declaration-specifiers declarator;
        declaration-specifiers declarator = initializer, ... ;
        declaration-specifiers declarator, ...;

    提取公共子表达式：如果第一个declarator后是{则是function-definition，否则是declaration
    */
    int storageClass = 0;
    int funSpec = 0;
    QualType ty = parseDeclarationSpecifiers(&storageClass, &funSpec);

    // 解析第一个声明符
    Declarator dl;
    parseDeclarator(dl);

    if (seq_->peek()->kind_ == TokenKind::LCurly_Brackets_)
    {/*function-definition: declaration-specifiers declarator declaration-listopt[ignore] compound-statement*/
        parseCompoundStmt();
        sema_->onActFunctionDecl();
    }
    else if (seq_->match(TokenKind::Assign_))
    {
        parseInitializer();
        while (seq_->match(TokenKind::Comma_)) 
        {
            parseInitDeclarator();
        }
        sema_->onActNamedDecl();
    }
    else if (seq_->match(TokenKind::Comma_))
    {
        parseInitDeclarator();
        while (seq_->match(TokenKind::Comma_)) 
        {
            parseInitDeclarator();
        }
        sema_->onActNamedDecl();
    }
    else if (!seq_->match(TokenKind::Semicolon_))
    {
        error(seq_->peek(), "unexcept symbol!");
    }
    return std::vector<Decl*>();
}

/*
(6.7) declaration:
    declaration-specifiers init-declarator-listopt ;

(6.7) init-declarator-list:
    init-declarator
    init-declarator-list , init-declarator
(6.7) init-declarator:
    declarator
    declarator = initializer
*/
bool Parser::parseDeclaration()
{
    return false;
}
/*-----------------------------parse expression---------------------------------*/
Expr *Parser::parsePrimaryExpr()
{
    Token *tk = seq_->next();
    switch (tk->kind_)
    {
    case TokenKind::identifier:
        /* code */
        return new DeclRefExpr();
    case TokenKind::String_Constant_:
        return new StringLiteral();
    case TokenKind::Character_Constant_:
        return new CharacterLiteral();
    case TokenKind::Numeric_Constant_:
        return new IntegerLiteral();
    case TokenKind::LParent_:
        Expr *retTk = parseExpr();
        seq_->expect(TokenKind::RParent_);
        return retTk;
    }
    return nullptr;
}


Expr *Parser::parseExpr()
{
    return nullptr;
}

/*6.8
 statement:
    labeled-statement
    compound-statement
    expression-statement
    selection-statement
    iteration-statement
    jump-statement
依次解析stmt*/
Stmt *Parser::parseStmt()
{
    switch (TokenKind::Case)
    {
    case TokenKind::Case:
        return parseCaseStmt();
    case TokenKind::Default:
        return parseDefaultStmt();
    case TokenKind::RCurly_Brackets_:
        return parseCompoundStmt();
    case TokenKind::Semicolon_:
        return nullptr;
    case TokenKind::If:
        return parseIfStmt();
    case TokenKind::Switch:
        return parseSwitchStmt();
    case TokenKind::While:
        return parseWhileStmt();
    case TokenKind::Do:
        return parseDoStmt();
    case TokenKind::For:
        return parseForStmt();
    case TokenKind::Goto:
        return parseGotoStmt();
    case TokenKind::Continue:
        return parseContinueStmt();
    case TokenKind::Break:
        return parseBreakStmt();
    case TokenKind::Return:
        return parseReturnStmt();
    case TokenKind::identifier:
        return parseLabelStmt();
    default:
        return parseExpr();
    }
}

LabelStmt *Parser::parseLabelStmt()
{
    return nullptr;
}

CaseStmt *Parser::parseCaseStmt()
{
    return nullptr;
}

DefaultStmt *Parser::parseDefaultStmt()
{
    return nullptr;
}

// 6.8.2 label语句
CompoundStmt *Parser::parseCompoundStmt()
{
    return nullptr;
}

// 6.8.4 selection语句
IfStmt *Parser::parseIfStmt()
{
    return nullptr;
}

SwitchStmt *Parser::parseSwitchStmt()
{
    return nullptr;
}

// 6.8.5 Iteration语句
WhileStmt *Parser::parseWhileStmt()
{
    return nullptr;
}

DoStmt *Parser::parseDoStmt()
{
    return nullptr;
}

ForStmt *Parser::parseForStmt()
{
    return nullptr;
}

// 6.6.6 jump 语句
GotoStmt *Parser::parseGotoStmt()
{
    return nullptr;
}

ContinueStmt *Parser::parseContinueStmt()
{
    return nullptr;
}

BreakStmt *Parser::parseBreakStmt()
{
    return nullptr;
}

ReturnStmt *Parser::parseReturnStmt()
{
    return nullptr;
}

void Parser::dumpAST()
{

} 

void Parser::dumpTokens()
{

}   
