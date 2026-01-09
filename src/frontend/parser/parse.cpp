#include "parse.h"
#include <sema.h>
#include <sstream>

// 状态机模式解析TypeSpecifier
enum TypeSpecCombine {
    // 符号
    COM_SIGNED =   SHORT | LONG | LONGLONG | INT | CHAR,
    COM_UNSIGNED = SHORT | LONG | LONGLONG | INT | CHAR,
    // 宽度
    COM_SHORT =  UNSIGNED | SIGNED | INT,
    COM_LONG =  UNSIGNED | SIGNED | LONG | INT | DOUBLE | _COMPLEX,
    // 类型
    COM_BOOL = N_TSPEC,
    COM_VOID = N_TSPEC,
    COM_CHAR = UNSIGNED | SIGNED,
    COM_INT =  UNSIGNED | SIGNED | SHORT | LONG | LONGLONG,
    COM_FLOAT =  _COMPLEX,
    COM_DOUBLE = LONG | _COMPLEX,
    COM_COMPLEX = FLOAT | DOUBLE | LONG,
    COM_RECORD = N_TSPEC,
    COM_ENUM= N_TSPEC,
    COM_TYPEDEFNAME = N_TSPEC,
};

ScopeManager::ScopeManager(Parser* p, Scope::ScopeType st)
: parent_(p) 
{
    parent_->m_systable->enterScope(st);
}
ScopeManager::~ScopeManager() 
{
    parent_->m_systable->exitScope();
}

Parser::Parser(const std::string& filename)
{
    m_source = std::make_shared<Source>(filename);
    m_tokenSeq = std::make_shared<TokenSequence>(scanner(m_source).tokenize());
    m_systable = std::make_shared<SymbolTableContext>();
    m_sema = std::make_shared<SemaAnalyzer>(m_systable);
}

Parser:: ~Parser()
{

}

void Parser::sytaxError(const std::string& val)
{
    sytaxError(m_tokenSeq->cur(), val);
}

void Parser::sytaxError(Token *tk, const std::string& val)
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
        << m_source->segline(tk->loc_)
        << std::string(tk->loc_.column, ' ') 
        << "^ "  
        << RED 
        << std::string(m_source->segline(tk->loc_).size() - tk->loc_.column - 2, '~')
        << CANCEL 
        << std::endl;
    #undef RED
    #undef CANCEL
    throw CCError(ss.str());
}

void Parser::semaError(const std::string& val)
{
    semaError(m_tokenSeq->cur(), val);
}

void Parser::semaError(Token *tk, const std::string& val)
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
        << m_source->segline(tk->loc_)
        << std::string(tk->loc_.column, ' ') 
        << "^ "  
        << RED 
        << std::string(m_source->segline(tk->loc_).size() - tk->loc_.column - 2, '~')
        << CANCEL 
        << std::endl;
    #undef RED
    #undef CANCEL
    std::cerr << ss.str();
}

//---------------------------------------------------------Expressions------------------------------------------------------------------------
/* primary-expression:
 identifier
 constant
 string-literal
 ( expression )
  generic-selection
*/
std::shared_ptr<Expr> Parser::parsePrimaryExpr()
{
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::Identifier:
    {
        auto sys = m_systable->LookupNormal(m_tokenSeq->cur()->value_);
        if (!sys)
            semaError("symbol undefined!");
        else if (!sys->getDecl())
            semaError("symbol declaration undefined!");
        else {
            auto node = std::make_shared<DeclRefExpr>();
            node->setType(sys->getType()->getQualType());
            node->setDecl(sys->getDecl());
            return node;
        }
    }    

    case TokenKind::Numeric_Constant_:
    {
        auto node = std::make_shared<IntegerLiteral>();
        node->setValue(std::atoll(m_tokenSeq->cur()->value_.c_str()));
        node->setType(m_systable->getBuiltTypeByTypeSpec(TypeSpecifier::LONGLONG));
        return node;
    }

    case TokenKind::Float_Constant:
    {
        auto node = std::make_shared<FloatingLiteral>();
        node->setValue(std::atof(m_tokenSeq->cur()->value_.c_str()));
        node->setType(m_systable->getBuiltTypeByTypeSpec(TypeSpecifier::DOUBLE));
        return node;
    }

    case TokenKind::Character_Constant_:
    {
        auto node = std::make_shared<CharacterLiteral>();
        node->setValue(m_tokenSeq->cur()->value_.at(0));
        node->setType(m_systable->getBuiltTypeByTypeSpec(TypeSpecifier::CHAR));
        return node;
    }

    case TokenKind::String_Constant_:
    {
        auto node = std::make_shared<StringLiteral>();
        node->setValue(m_tokenSeq->cur()->value_);
        return node;
    }
    
    case TokenKind::LParent_:
    {
        auto child = parseExpr();
        m_tokenSeq->expect(TokenKind::RParent_);
        auto node = std::make_shared<ParenExpr>();
        node->setSubExpr(child);
        node->setType(child->getType());
        return node;
    }

    default:
        sytaxError(m_tokenSeq->cur(), "unexpect PrimaryExpr!");
        break;
    }
    return nullptr;
}

/* (6.5.1.1) gegneric-selection:
 _Generic ( assignment-expression , generic-assoc-list
 )
 (6.5.1.1) generic-assoc-list:
 generic-association
 generic-assoc-list , generic-association
*/
std::shared_ptr<Expr> Parser::parseGenericSelection()
{
    m_tokenSeq->expect(TokenKind::T_Generic);
    m_tokenSeq->expect(TokenKind::LParent_);
    parseAssignExpr();
    while (m_tokenSeq->match(TokenKind::Comma_)) {
        parseGenericAssociation();
    }
    m_tokenSeq->expect(TokenKind::RParent_);
    return nullptr;
}

/*(6.5.1.1) generic-association:
 type-name : assignment-expression
 default : assignment-expression
*/
std::shared_ptr<Expr> Parser::parseGenericAssociation()
{
    if (m_tokenSeq->match(TokenKind::Default)) {

    }
    else {
        parseTypeName();
    }
    m_tokenSeq->match(TokenKind::Colon_);
    parseAssignExpr();
    return nullptr;
}

/* (6.5.2) postfix-expression:
    消除左递归文法后:
    postfix-expression = (( type-name ){ initializer-list } | ( type-name ){ initializer-list ,} | primary-expression）tail
    tail = ( [ expression ] | ( argument-expression-listopt ) | . identifier | -> identifier | ++ | -- ) tail | e
    解析重点：
    复合字面值和primary都具备 （type-name）(expr)等括号表达式形式
*/
std::shared_ptr<Expr> Parser::parsePostfixExpr()
{
    std::shared_ptr<Expr> node = nullptr;
    if (m_tokenSeq->test(TokenKind::LParent_)) {
        node = parseParenExpr();
    }
    else {
        node = parsePrimaryExpr();
    }
    while (true) {
        if (m_tokenSeq->match(TokenKind::LSquare_Brackets_)) {
            auto array = std::make_shared<ArraySubscriptExpr>();
            array->setIndexExpr(parseExpr());
            array->setBaseExpr(node);
            m_tokenSeq->expect(TokenKind::RSquare_Brackets_);
        }
        else if (m_tokenSeq->match(TokenKind::LParent_)) {
            auto args = std::make_shared<CallExpr>();
            args->setParams(parseArgListExpr());
            args->setCallee(node);
            m_tokenSeq->expect(TokenKind::RParent_);
        }
        else if (m_tokenSeq->match(TokenKind::Dot_)) {
            // TODO 成员运算符
        }
        else if (m_tokenSeq->match(TokenKind::Arrow_)) {
            // TODO 成员运算符
        }
        // 自增表达式
        else if (m_tokenSeq->match(TokenKind::Increment_)) {
            auto inc = std::make_shared<UnaryOpExpr>();
            inc->setOpCode(UnaryOpExpr::Post_Increment_);
            inc->setSubExpr(node);
            node = inc;
        }
        // 自减表达式
        else if (m_tokenSeq->match(TokenKind::Decrement_)) {
            auto dec = std::make_shared<UnaryOpExpr>();
            dec->setOpCode(UnaryOpExpr::Post_Decrement_);
            dec->setSubExpr(node);
            node = dec;
        }
        else {
            break;
        }
    }
    return node;
}

std::vector<std::shared_ptr<Expr>> Parser::parseArgListExpr()
{
    std::vector<std::shared_ptr<Expr>> res;
    res.push_back(parseAssignExpr());
    while (m_tokenSeq->match(TokenKind::Comma_)) {
        res.push_back(parseAssignExpr());
    }
    return res;
}

/*
 (6.5.3) unary-expression:
    postfix-expression
    ++ unary-expression
    -- unary-expression
    unary-operator cast-expression
    sizeof unary-expression
    sizeof ( type-name )

unary-operator: one of
    & * +- ~ !
*/
std::shared_ptr<Expr> Parser::parseUnaryExpr()
{
    auto node = std::make_shared<UnaryOpExpr>();
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::Increment_:
        node->setOpCode(UnaryOpExpr::Pre_Increment_);
        node->setSubExpr(parseUnaryExpr());
        break;

    case TokenKind::Decrement_:
        node->setOpCode(UnaryOpExpr::Pre_Decrement_);
        node->setSubExpr(parseUnaryExpr());
        break;

    case TokenKind::BitWise_AND_:
        node->setOpCode(UnaryOpExpr::BitWise_AND_);
        node->setSubExpr(parseCastExpr());
        break;

    case TokenKind::Multiplication_:
        node->setOpCode(UnaryOpExpr::Multiplication_);
        node->setSubExpr(parseCastExpr());
        break;

    case TokenKind::Addition_:
        node->setOpCode(UnaryOpExpr::Addition_);
        node->setSubExpr(parseCastExpr());
        break;

    case TokenKind::Subtraction_:
        node->setOpCode(UnaryOpExpr::Subtraction_);
        node->setSubExpr(parseCastExpr());
        break;

    case TokenKind::BitWise_NOT_:
        node->setOpCode(UnaryOpExpr::BitWise_NOT_);
        node->setSubExpr(parseCastExpr());
        break;

    case TokenKind::Logical_NOT_:
        node->setOpCode(UnaryOpExpr::Logical_NOT_);
        node->setSubExpr(parseCastExpr());
        break;
    
    case TokenKind::Sizeof:
        break;
    
    case TokenKind::Alignof:
        break;

    default:
        return parsePostfixExpr();
    }
    return node;
}

/*(6.5.4) cast-expression:
 unary-expression
 ( type-name ) cast-expression
*/
std::shared_ptr<Expr> Parser::parseCastExpr()
{
    return parseUnaryExpr();
}

std::shared_ptr<Expr> Parser::parseMultiExpr()
{
    auto node = parseCastExpr();
    while (true) {
        auto rex = std::make_shared<BinaryOpExpr>();
        if (m_tokenSeq->match(TokenKind::Multiplication_)) {
            rex->setOpCode(BinaryOpExpr::Multiplication_);
        } 
        else if (m_tokenSeq->match(TokenKind::Division_)) {
            rex->setOpCode(BinaryOpExpr::Division_);
        } 
        else if (m_tokenSeq->match(TokenKind::Modulus_)) {
            rex->setOpCode(BinaryOpExpr::Modulus_);
        } 
        else {
            break;
        }
        rex->setLExpr(node);
        rex->setRExpr(parseCastExpr());
        node = rex;
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseAddExpr()
{
    auto node = parseMultiExpr();
    while (true) {
        auto rex = std::make_shared<BinaryOpExpr>();
        if (m_tokenSeq->match(TokenKind::Addition_)) {
            rex->setOpCode(BinaryOpExpr::Addition_);
        } else if (m_tokenSeq->match(TokenKind::Subtraction_)) {
            rex->setOpCode(BinaryOpExpr::Subtraction_);
        } else {
            break;
        }
        rex->setLExpr(node);
        rex->setRExpr(parseMultiExpr());
        node = rex;
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseShiftExpr()
{
    auto node = parseAddExpr();
    while (true) {
        auto rex = std::make_shared<BinaryOpExpr>();
        if (m_tokenSeq->match(TokenKind::LShift_)) {
            rex->setOpCode(BinaryOpExpr::LShift_);
        } else if (m_tokenSeq->match(TokenKind::RShift_)) {
            rex->setOpCode(BinaryOpExpr::RShift_);
        } else {
            break;
        }
        rex->setLExpr(node);
        rex->setRExpr(parseAddExpr());
        node = rex;
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseRelationalExpr()
{
    auto node = parseShiftExpr();
    while (true) {
        auto rex = std::make_shared<BinaryOpExpr>();
        if (m_tokenSeq->match(TokenKind::Less_)) {
            rex->setOpCode(BinaryOpExpr::Less_);
        } else if (m_tokenSeq->match(TokenKind::Less_Equal_)) {
            rex->setOpCode(BinaryOpExpr::Less_Equal_);
        } else if (m_tokenSeq->match(TokenKind::Greater_)) {
            rex->setOpCode(BinaryOpExpr::Greater_);
        } else if (m_tokenSeq->match(TokenKind::Greater_Equal_)) {
            rex->setOpCode(BinaryOpExpr::Greater_Equal_);
        } else  {
            break;
        }
        rex->setLExpr(node);
        rex->setRExpr(parseShiftExpr());
        node = rex;
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseEqualExpr()
{
    auto node = parseRelationalExpr();
    while (true) {
        auto rex = std::make_shared<BinaryOpExpr>();
        if (m_tokenSeq->match(TokenKind::Equality_)) {
            rex->setOpCode(BinaryOpExpr::Equality_);
        } else if (m_tokenSeq->match(TokenKind::Inequality_)) {
            rex->setOpCode(BinaryOpExpr::Inequality_);
        } else {
            break;
        }
        rex->setLExpr(node);
        rex->setRExpr(parseRelationalExpr());
        node = rex;
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseBitANDExpr()
{
    auto node = parseEqualExpr();
    while (m_tokenSeq->match(TokenKind::BitWise_AND_)) {
        auto rex = std::make_shared<BinaryOpExpr>();
        rex->setOpCode(BinaryOpExpr::BitWise_AND_);
        rex->setLExpr(node);
        rex->setRExpr(parseEqualExpr());
        node = rex;
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseBitXORExpr()
{
    auto node = parseBitANDExpr();
    while (m_tokenSeq->match(TokenKind::BitWise_XOR_)) {
        auto rex = std::make_shared<BinaryOpExpr>();
        rex->setOpCode(BinaryOpExpr::BitWise_XOR_);
        rex->setLExpr(node);
        rex->setRExpr(parseBitANDExpr());
        node = rex;
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseBitORExpr()
{
    auto node = parseBitXORExpr();
    while (m_tokenSeq->match(TokenKind::BitWise_OR_)) {
        auto rex = std::make_shared<BinaryOpExpr>();
        rex->setOpCode(BinaryOpExpr::BitWise_OR_);
        rex->setLExpr(node);
        rex->setRExpr(parseBitXORExpr());
        node = rex;
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseLogicalANDExpr()
{
    auto node = parseBitORExpr();
    while (m_tokenSeq->match(TokenKind::Logical_AND_)) {
        auto rex = std::make_shared<BinaryOpExpr>();
        rex->setOpCode(BinaryOpExpr::Logical_AND_);
        rex->setLExpr(node);
        rex->setRExpr(parseBitORExpr());
        node = rex;
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseLogicalORExpr()
{
    auto node = parseLogicalANDExpr();
    while (m_tokenSeq->match(TokenKind::Logical_OR_)) {
        auto rex = std::make_shared<BinaryOpExpr>();
        rex->setOpCode(BinaryOpExpr::Logical_OR_);
        rex->setLExpr(node);
        rex->setRExpr(parseLogicalANDExpr());
        node = rex;
    }
    return node;
}

std::shared_ptr<Expr> Parser::parseConditionalExpr()
{
    auto node = parseLogicalORExpr();
    if (m_tokenSeq->match(TokenKind::Conditional_)) {
        auto rex = std::make_shared<ConditionalExpr>();
        rex->setCond(node);
        rex->setElse(parseExpr());
        m_tokenSeq->expect(TokenKind::Colon_);
        rex->setThen(parseConditionalExpr());
        node = rex;
    }
    return node;
}

/*(6.5.16) assignment-expression:
 conditional-expression
 unary-expression assignment-operator assignment-expression
 分析难点：assign有两个的候选式，无法判断使用哪个候选式，文法改写成：
 conditional-expression
 conditional-expression assignment-operator assignment-expression
 conditional-expression包含了unary-expression， 语义分析阶段纠正
 (6.5.16) assignment-operator: one of
 = *= /= %= +=-= <<= >>= &= ^= |=
*/
std::shared_ptr<Expr> Parser::parseAssignExpr()
{
    auto node = std::make_shared<BinaryOpExpr>();
    node->setLExpr(parseConditionalExpr());
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::Assign_:
        node->setOpCode(BinaryOpExpr::Assign_);
        break;

    case TokenKind::Mult_Assign_:
        node->setOpCode(BinaryOpExpr::Mult_Assign_);
        break;

    case TokenKind::Div_Assign_:
        node->setOpCode(BinaryOpExpr::Div_Assign_);
        break;

    case TokenKind::Mod_Assign_:
        node->setOpCode(BinaryOpExpr::Mod_Assign_);
        break;

    case TokenKind::Add_Assign_:
        node->setOpCode(BinaryOpExpr::Add_Assign_);
        break;

    case TokenKind::Sub_Assign_:
        node->setOpCode(BinaryOpExpr::Sub_Assign_);
        break;

    case TokenKind::LShift_Assign_:
        node->setOpCode(BinaryOpExpr::LShift_Assign_);
        break;

    case TokenKind::RShift_Assign_:
        node->setOpCode(BinaryOpExpr::RShift_Assign_);
        break;

    case TokenKind::BitWise_AND_Assign_:
        node->setOpCode(BinaryOpExpr::BitWise_AND_Assign_);
        break;

    case TokenKind::BitWise_XOR_Assign_:
        node->setOpCode(BinaryOpExpr::BitWise_XOR_Assign_);
        break;

    case TokenKind::BitWise_OR_Assign_:
        node->setOpCode(BinaryOpExpr::BitWise_OR_Assign_);
        break;
        
    default:
        sytaxError("expect assignment-operator, but not!");
        break;
    }
    node->setRExpr(parseAssignExpr());
    return node;
}

/*
 (6.5.17) expression:
 assignment-expression
 expression , assignment-expression
*/
std::shared_ptr<Expr> Parser::parseExpr()
{
    auto node = parseAssignExpr();
    while (m_tokenSeq->match(TokenKind::Comma_)) {
        auto rex = std::make_shared<BinaryOpExpr>();
        rex->setOpCode(BinaryOpExpr::Comma);
        rex->setLExpr(node);
        rex->setRExpr(parseAssignExpr());
        node = rex;
    }
    return node;
}

/*(6.6) constant-expression:
 conditional-expression
*/
std::shared_ptr<Expr> Parser::parseConstansExpr()
{
    return parseConditionalExpr();
}

/* primary-expr: ( expr )
   posftix-expr： （type-name）{...}
   cast-expr: (type-name) cast-expr
*/
std::shared_ptr<Expr> Parser::parseParenExpr()
{
    m_tokenSeq->expect(TokenKind::LParent_);
    if (m_systable->isTypeName(m_tokenSeq->peek())) {
        QualType qt = parseTypeName();
        m_tokenSeq->expect(TokenKind::RParent_);
        if (m_tokenSeq->match(TokenKind::LCurly_Brackets_)) {
            // TODO
        }
        else {
            // TODO
        }
    }
    else {
        auto node = std::make_shared<ParenExpr>();
        node->setSubExpr(parseExpr());
        m_tokenSeq->expect(TokenKind::RParent_);
        return node;
    }
    return nullptr;
}

//---------------------------------------------------------Declarations------------------------------------------------------------------------
/* (6.7) declaration:
 declaration-specifiers init-declarator-listopt ;
 若类型declSpec是type-name, 则init-declarator不需要解析。
*/
DeclGroup Parser::parseDeclaration()
{
    DeclGroup res;
    auto declarator = parseDeclarationSpec();
    if (m_tokenSeq->match(TokenKind::Semicolon_)) {
        // 枚举，结构体和typedef返回，不加入到全局声明
        return res;
    }

    // 解析第一个声明符
    auto decl = parseInitDeclarator(declarator);
    // 检测是否是函数
    if (decl->getKind() == AstNode::NK_FunctionDecl) {
        // 判断是否是函数定义是，直接返回
        if (m_tokenSeq->test(TokenKind::LCurly_Brackets_)) {
            dynamic_cast<FunctionDecl*>(decl.get())->setBody(parseCompoundStmt());
            res.push_back(decl);
            return res;
        }
        // 不是函数定义时，添加到res
        res.push_back(decl);
    }
    // 解析后续的变量声明
    while (m_tokenSeq->match(TokenKind::Comma_)) {
            res.push_back(parseInitDeclarator(declarator));
    }
    m_tokenSeq->expect(TokenKind::Semicolon_);
    return res;
}

Declarator Parser::parseDeclarationSpec()
{
    Declarator res;
    int ts = 0, tq = 0;
    while (true)
    {
        TokenKind tkind = m_tokenSeq->next()->kind_;
        switch (tkind)
        {
        // (6.7.1) storage class specifier
        case TokenKind::Typedef:
            if (res.getStorageClass() != 0) {
                sytaxError("duplication storageclass!");
            }
            res.setStorageClass(StorageClass::TYPEDEF);
            res.setKind(Declarator::DK_TYPEDEFNAME);
            break;
        case TokenKind::Extern:
            if (res.getStorageClass() != 0) {
                sytaxError("duplication storageclass!");
            }
            res.setStorageClass(StorageClass::EXTERN);
            break;
        case TokenKind::Static:
            if (res.getStorageClass() != 0) {
                sytaxError("duplication storageclass!");
            }
            res.setStorageClass(StorageClass::STATIC);
            break;
        case TokenKind::Auto:
            if (res.getStorageClass() != 0) {
                sytaxError("duplication storageclass!");
            }
            res.setStorageClass(StorageClass::AUTO);
            break;
        case TokenKind::Register:
            if (res.getStorageClass() != 0) {
                sytaxError("duplication storageclass!");
            }
            res.setStorageClass(StorageClass::REGISTER);
            break;

        // (6.7.2) type specifiers
        // 使用状态机解析TypeSpecifier
        case TokenKind::T_Bool:
            if (ts & ~COM_BOOL) {
                sytaxError("unexpect bool!");
            }
            ts |= TypeSpecifier::_BOOL;
            break;

        case TokenKind::T_Complex:
            if (ts & ~COM_COMPLEX) {
                sytaxError("unexpect complex!");
            }
            ts |= TypeSpecifier::_COMPLEX;
            break;

        case TokenKind::Void:
            if (ts & ~COM_VOID) {
                sytaxError("unexpect void!");
            }
            ts |= TypeSpecifier::VOID;
            break;

        case TokenKind::Char:
            if (ts & ~COM_CHAR) {
                sytaxError("unexpect char!");
            }
            ts |= TypeSpecifier::CHAR;
            break;

        case TokenKind::Int:
            if (ts & ~COM_INT) {
                sytaxError("unexpect int!");
            }
            ts |= TypeSpecifier::INT;
            break;

        case TokenKind::Float:
            if (ts & ~COM_FLOAT) {
                sytaxError("unexpect float!");
            }
            ts |= TypeSpecifier::FLOAT;
            break;

        case TokenKind::Double:
            if (ts & ~COM_DOUBLE) {
                sytaxError("unexpect double!");
            }
            ts |= TypeSpecifier::DOUBLE;
            break;

        case TokenKind::Signed:
            if (ts & ~COM_SIGNED) {
                sytaxError("unexpect signed!");
            }
            ts |= TypeSpecifier::SIGNED;
            break;

        case TokenKind::Unsigned:
            if (ts & ~COM_UNSIGNED) {
                sytaxError("unexpect unsigned!");
            }
            ts |= TypeSpecifier::UNSIGNED;
            break;

        case TokenKind::Short:
            if (ts & ~COM_SHORT) {
                sytaxError("unexpect short!");
            }
            ts |= TypeSpecifier::SHORT;
            break;

        case TokenKind::Long:
            if (ts & ~COM_LONG) {
                sytaxError("unexpect long!");
            }
            // 筛除不合法
            if (ts & LONG) {
                // 没有long long double/ long long complex形式
                if ( ts & DOUBLE || ts & _COMPLEX) {
                    sytaxError("unexpect long!");
                } else {
                    ts &= ~LONG;
                    ts |= LONGLONG;
                }
            } else {
                ts |= LONG;
            } 
            break;

        // (6.7.2) type-specifier->struct-or-union-specifier
        case TokenKind::Struct:
        case TokenKind::Union:
        {
            bool isStruct = m_tokenSeq->cur()->kind_ == TokenKind::Struct;
            if (ts & ~COM_RECORD) {
                sytaxError("unexpect struct or union!");
            }
            res.setType(QualType(parseStructOrUnionSpec(isStruct), tq));
            res.setKind(Declarator::DK_RECORD);
            return res;
        }

        // (6.7.2) type-specifier->enum-specifier
        case TokenKind::Enum:
            if (ts & ~COM_ENUM) {
                sytaxError("unexpect enum!");
            }
            res.setType(QualType(parseEnumSpec(), tq));
            res.setKind(Declarator::DK_ENUM);
            return res;

        //(6.7.3) type-qualifier:
        case TokenKind::Const:    tq |= TypeQualifier::CONST; break;
        case TokenKind::Volatile: tq |= TypeQualifier::VOLATILE; break;
        case TokenKind::Restrict: tq |= TypeQualifier::RESTRICT; break;

        // (6.7.4) function-specifier
        case TokenKind::Inline:
            res.setFuncSpec(FuncSpecifier::INLINE);
            res.setKind(Declarator::DK_FUNC);
            break;

        // (6.7.7) typedef-name 判断当前是否已有其他方式
        case TokenKind::Identifier:
            // 遇到标识符后，判断是否有基本类型
            if (ts) {
                res.setType(QualType(m_systable->getBuiltTypeByTS(ts), tq));
                m_tokenSeq->reset();
            }
            // 判断之前是否已经有类型
            else if (!res.getType().isNull()) {
                m_tokenSeq->reset();
            }
            // 判断标识符是否是类型
            else if (m_systable->isTypeName(m_tokenSeq->cur())){
                auto sys = m_systable->LookupNormal(m_tokenSeq->cur()->value_);
                if (sys) {
                    auto ty = sys->getType();
                    res.setType(QualType(ty.getPtr(), ty.getQual()|tq));
                }
            }
            else {
                sytaxError("expect type, but not!");
                break;
            }

        default:
            // 遇到了标识符或者其他
            if (res.getType().isNull()) {
                sytaxError("incomplete type specifier!");
            }
            return res;
        }
    }
    return res;
}

/*init-declarator:
 declarator
 declarator = initializer
*/
std::shared_ptr<DeclaratorDecl> Parser::parseInitDeclarator(Declarator dc)
{
    parseDeclarator(dc);
    std::shared_ptr<Expr> initExpr = nullptr;
    if (m_tokenSeq->match(TokenKind::Assign_)) {
        initExpr = parseInitializer();
    }
    // 判断当前是否处于函数原型作用域
    bool isFuncProto = (m_systable->isScopeType(Scope::FUNC_PROTOTYPE));
    Decl* res = new VarDecl(dc.getName(), m_systable->getCurScope(), dc.getType(), dc.getStorageClass(), initExpr);

    // 插入符号表
    Symbol* sym = nullptr;
    if (!dc.getName().empty()) {
        sym = nullptr;//sys_->insertNormal(dc.getName(), dc.getType(), res);
    }
    return res;
}

void Parser::parseStorageClassSpec(StorageClass val, int* sc)
{
    if (sc == nullptr) {
        sytaxError("expect not storageclass, but has!");
        return;
    } 
    else if (sc != 0) {
        sytaxError("duplication storageclass!");
        return;
    }
    *sc |= val;
}

/* (6.7.2.1) struct-or-union-specifier:
 struct-or-union identifieropt { struct-declaration-list }
 struct-or-union identifier
*/
std::shared_ptr<RecordType> Parser::parseStructOrUnionSpec(bool isStruct)
{
    //符号解析
    std::string key;
    if (m_tokenSeq->match(TokenKind::Identifier)) {
        key = m_tokenSeq->cur()->value_;
    }

    Symbol* sym = m_systable->lookup(Symbol::RECORD, key);
    // UDT定义
    if (m_tokenSeq->match(TokenKind::LCurly_Brackets_)) {
        // 符号表没查找到:第一次定义
        if (!sym) {
            Type* ty = nullptr;//RecordType::NewObj(isStruct, nullptr);
            if (!key.empty()) { // 匿名对象不插入符号表
                sym = m_systable->insertRecord(key, ty, nullptr);
            }
            return parseStructDeclarationList(sym);
        }
        // 符号表查找到了但是类型定义不完整：存在前向声明
        else if (!sym->getType()->isCompleteType()) {
            return parseStructDeclarationList(sym);
        }
        // 符号表查找到了并且类型定义完整：重复定义
        else {
            sytaxError("redefined struct or union!");
            return nullptr;
        }
    }
    // UDT前向声明 struct test;
    // struct test *p;
    // 非定义情况下使用UDT必须要有声明符
    if (key.empty()) {
        sytaxError("struct or union need identifier, but not!");
        return nullptr;
    }
    if (sym) {
        return nullptr;//sym->getType();
    }
    Type* ty = nullptr; //::NewObj(isStruct, nullptr);
    m_systable->insertRecord(key, ty, nullptr);
    return ty;
}

/* (6.7.2.1) struct-declaration-list:
 struct-declaration
 struct-declaration-list struct-declaration
 struct-declaration:
 specifier-qualifier-list struct-declarator-list ;
 解析结构体成员
*/
QualType Parser::parseStructDeclarationList(Symbol* sym)
{
    ScopeManager scm(this, Scope::BLOCK);
    if (!sym) {
        return nullptr;
    }
    RecordType* ty = nullptr;//<RecordType*>(sym->getType());
    RecordDecl* dc = nullptr;//RecordDecl::NewObj(sym, true, ty->isStructType());
    do {
        QualType qt = parseSpecQualList();
        DeclGroup path = parseStructDeclaratorList(qt, dc);
        //dc->addField(path);
        m_tokenSeq->match(TokenKind::Semantics);
    }while (m_tokenSeq->test(TokenKind::RCurly_Brackets_));
    ty->setTagDecl(dc);
    return ty;
}

/* (6.7.2.1) specifier-qualifier-list:
 type-specifier specifier-qualifier-listopt
 type-qualifier specifier-qualifier-listopt
*/
QualType Parser::parseSpecQualList()
{
    return parseDeclarationSpec(nullptr, nullptr);
}

/* (6.7.2.1) struct-declarator-list:
 struct-declarator
 struct-declarator-list , struct-declarator

  (6.7.2.1) struct-declarator:
 declarator
 declaratoropt : constant-expression
*/
DeclGroup Parser::parseStructDeclaratorList(QualType qt, Decl* parent)
{
    DeclGroup res;
    do {
        Decl* dc = nullptr;
        Expr* initEx = nullptr;
        if (m_tokenSeq->match(TokenKind::Colon_)) {
            initEx = parseConstansExpr();
            dc = nullptr;//FieldDecl::NewObj(nullptr, qt, parent, 0);
        }
        else {
            //NamedDecl* tmp = dynamic_cast<NamedDecl*>(parseDeclarator(qt, 0, 0));
            NamedDecl* tmp = nullptr;
            if (m_tokenSeq->match(TokenKind::Colon_)) {
                initEx = parseConstansExpr();
            }
            dc = nullptr;//FieldDecl::NewObj(tmp->getSymbol(), qt, parent, 0);
        }
        res.push_back(dc);

    }while (m_tokenSeq->match(TokenKind::Semicolon_));
    return res;
}

/* (6.7.2.2) enum-specifier:
 enum identifieropt { enumerator-list }
 enum identifieropt { enumerator-list ,}
 enum identifier
*/
std::shared_ptr<EnumType> Parser::parseEnumSpec()
{
    // 符号解析
    std::string key;
    if (m_tokenSeq->match(TokenKind::Identifier)) {
        key = m_tokenSeq->cur()->value_;
    }

    // 枚举定义解析
    Symbol* sym = m_systable->lookup(Symbol::RECORD, key);
    if (m_tokenSeq->match(TokenKind::LCurly_Brackets_)) {
        // 符号表没有查找到，第一次定义
        if (!sym) {
            Type* ty = EnumType::NewObj(nullptr);
            if (key.empty()) { // 匿名对象不插入符号表
                m_systable->insertRecord(key, ty, nullptr);
            }
            return parseEnumeratorList(nullptr, ty);
        }
        // 符号表查找到了但是未定义
        else if (!sym->getType()->isCompleteType()) {
            Type* ty = nullptr;//->getType();
            return parseEnumeratorList(sym, ty);
        }
        // 其他情况：符号表查找到了但是已经定义了：重定义错误
        else {
            sytaxError("redefined enum identifier!");
            return nullptr;
        }
    }
    // 枚举声明或使用
    // 必须要定义符号
    if (key.empty()) {
        sytaxError("expect enum identifier, but not!");
        return nullptr;
    }
    // 返回类型，若由符号则返回，否则创建
    if (!sym) {
        return nullptr;// sym->getType();
    }
    Type* ty = EnumType::NewObj(nullptr);
    m_systable->insertRecord(key, ty, nullptr);
    return ty;
}

/*  (6.7.2.2) enumerator-list:
 enumerator
 enumerator-list , enumerator
*/
QualType Parser::parseEnumeratorList(Symbol* sym, Type* ty)
{
    // 打开块作用域
    ScopeManager scm(this, Scope::BLOCK);
    EnumDecl* dc = nullptr;//::NewObj(sym, true);
    while (true) {
        dc->addConstant(parseEnumerator(QualType()));
        // 匹配到逗号
        if (m_tokenSeq->match(TokenKind::Comma_)) {
            if (m_tokenSeq->match(TokenKind::RCurly_Brackets_)) {
                break;
            }
            continue;
        }
        // 未匹配到逗号，则必定结束
        else {
            m_tokenSeq->expect(TokenKind::RCurly_Brackets_);
            break;
        }
    }
    // 匿名对象则创建类型
    EnumType* t = dynamic_cast<EnumType*>(ty);
    t->setTagDecl(dc);
    return ty;
}

/*(6.7.2.2) enumerator:
 enumeration-constant
 enumeration-constant = constant-expression
  (6.4.4.3) enumeration-constant:
 identifier
*/
std::shared_ptr<EnumConstantDecl> Parser::parseEnumerator(QualType qt)
{
    // 解析符号
    m_tokenSeq->expect(TokenKind::Identifier);
    Symbol* sym = nullptr;
    // 解析表达式
    Expr* ex = nullptr;
    if (m_tokenSeq->match(TokenKind::Assign_)) {
        ex = parseConstansExpr();
    }
    EnumConstantDecl* dc = nullptr;//EnumConstantDecl::NewObj(sym, ex);
    // 插入符号表
    m_systable->insertMember(m_tokenSeq->cur()->value_, nullptr, nullptr);
    return nullptr;
}

/* (6.7.3) type-qualifier:
 const
 restrict
 volatile
*/
int Parser::parseTypeQualList()
{
    //(6.7.3) type-qualifier:
    int res = 0;
    while (true) {
        switch (m_tokenSeq->peek()->kind_)
        {
        case TokenKind::Const:   
            res |= TypeQualifier::CONST;
            break;
        case TokenKind::Volatile:
            res |= TypeQualifier::VOLATILE;
            break;
        case TokenKind::Restrict:
            res |= TypeQualifier::RESTRICT;
            break;
        default:
            return res;
        }
        m_tokenSeq->next();
    }
    return res;
}

/*(6.7.4) function-specifier:
 inline
*/
void Parser::parseFunctionSpec(FuncSpecifier val, int* fs)
{
    if (fs == nullptr) {
        sytaxError("expect not function specifier, but has!");
        return;
    } 
    else if (fs != 0) {
        sytaxError("duplication function specifier!");
        return;
    }
    *fs |= val;
}

/*declarator:
 pointeropt direct-declarator
*/
void Parser::parseDeclarator(Declarator& dc)
{
    // 计算pointer指针类型
    auto base = parsePointer(dc.getType());
    dc.setType(base);
    // （ declarator ）
    if (m_tokenSeq->match(TokenKind::Identifier)) {
        dc.setName(m_tokenSeq->cur()->value_);
    }
    else if (m_tokenSeq->match(TokenKind::LParent_)) {
        parseDeclarator(dc);
        m_tokenSeq->expect(TokenKind::RParent_);
    }
    else {
        sytaxError("expect declarator, but not!");
    }
    // 解析声明符后缀
    while (true)
    {
        if (m_tokenSeq->match(TokenKind::LParent_)) {
            m_systable->enterFuncPrototypeScope();
            auto paramList = parseParameterList();
            m_systable->exitScope();
            m_tokenSeq->expect(TokenKind::RParent_);
            dc.setType(FunctionType::NewObj(base, false, false, paramList));
            // 更新声明符的类型
            dc.setType(modifyBaseType(base, base, dc.getType()));
        }
        else if (m_tokenSeq->match(TokenKind::LSquare_Brackets_)) {
            dc.setType(ArrayType::NewObj(dc.getType(), parseArrayLen()));
            // 更新声明符的类型
            dc.setType(modifyBaseType(base, base, dc.getType()));
        }
        else {
            break;
        }
    }
    
}
QualType Parser::modifyBaseType(QualType oldBase, QualType newBase, QualType curType)
{
    if (curType == oldBase) {
        return newBase;
    }
    curType->setQualType(modifyBaseType(oldBase, newBase, curType->getQualType()));
    return curType;
}

std::shared_ptr<Expr> Parser::parseArrayLen()
{
    return nullptr;
}

QualType Parser::parseFuncOrArrayDeclarator(QualType base)
{
    return base;
}

/*(6.7.5) pointer:
 * type-qualifier-listopt
 * type-qualifier-listopt pointer
*/
QualType Parser::parsePointer(QualType qt)
{ 
    while (m_tokenSeq->match(TokenKind::Multiplication_)) {
        auto ty = std::make_shared<PointerType>(qt);
        qt = QualType(ty, parseTypeQualList());
    }
    return qt;
}

void Parser::parseParameterTypeList()
{
    parseParameterList();
    if (m_tokenSeq->match(TokenKind::Comma_)) {
        m_tokenSeq->expect(TokenKind::Ellipsis_);
    }
}

std::vector<ParmVarDecl*> Parser::parseParameterList()
{
    parseParameterDeclaration();
    while (m_tokenSeq->match(TokenKind::Comma_)) {
        parseParameterDeclaration();
    }
    return std::vector<ParmVarDecl*>();
}

/* (6.7.5) parameter-declaration:
 declaration-specifiers declarator
 declaration-specifiers abstract-declaratoropt
*/
void Parser::parseParameterDeclaration()
{
    QualType qt = parseDeclarationSpec(nullptr, nullptr);
    if (m_tokenSeq->match(TokenKind::Identifier)) {
        // 解析标识符
        m_tokenSeq->expect(TokenKind::Identifier);
    }
    else if (m_tokenSeq->match(TokenKind::LParent_)) {
        // 解析括号表达式
        m_tokenSeq->expect(TokenKind::LParent_);
        //parseDeclarator(qt);
        m_tokenSeq->expect(TokenKind::RParent_);
    }
    else {
        parseAbstractDeclarator();
    }
    // 解析可选的函数参数列表
    if (m_tokenSeq->match(TokenKind::LParent_)) {
        m_tokenSeq->expect(TokenKind::LParent_);
        parseParameterTypeList();
        m_tokenSeq->expect(TokenKind::RParent_);
    }
}

/* (6.7.5) identifier-list:
 identifier
 identifier-list , identifier
*/
void Parser::parseIdentifierList()
{
    m_tokenSeq->expect(TokenKind::Identifier);
    while (m_tokenSeq->match(TokenKind::Comma_)) {
        m_tokenSeq->expect(TokenKind::Identifier);
    }

}

/*(6.7.6) type-name:
 specifier-qualifier-list abstract-declaratoropt
*/
QualType Parser::parseTypeName()
{
    parseSpecQualList();
    parseAbstractDeclarator();
    return QualType();
}

/* (6.7.6) abstract-declarator:
 pointer
 pointeropt direct-abstract-declarator
*/
void Parser::parseAbstractDeclarator()
{
    //parsePointer();
    parseDirectAbstractDeclarator();
}

void Parser::parseDirectAbstractDeclarator()
{
    if (m_tokenSeq->match(TokenKind::LParent_)) {
        m_tokenSeq->expect(TokenKind::LParent_);
        parseAbstractDeclarator();
        m_tokenSeq->expect(TokenKind::RParent_);
    }
    else if (m_tokenSeq->match(TokenKind::LSquare_Brackets_)) {
        m_tokenSeq->expect(TokenKind::LSquare_Brackets_);
        if (m_tokenSeq->match(TokenKind::RSquare_Brackets_)) {
            // 数组长度不确定
            m_tokenSeq->expect(TokenKind::RSquare_Brackets_);
        } else {
            parseConstansExpr();
            m_tokenSeq->expect(TokenKind::RSquare_Brackets_);
        }
    }
    else if (m_tokenSeq->match(TokenKind::LCurly_Brackets_)) {
        // 解析函数声明
        m_tokenSeq->expect(TokenKind::LCurly_Brackets_);
        parseParameterTypeList();
        m_tokenSeq->expect(TokenKind::RCurly_Brackets_);
    }
    else {
        sytaxError("expect direct abstract declarator, but not!");
    }
    // 解析可选的函数参数列表
    if (m_tokenSeq->match(TokenKind::LParent_)) {
        m_tokenSeq->expect(TokenKind::LParent_);
        parseParameterTypeList();
        m_tokenSeq->expect(TokenKind::RParent_);
    }
}

/* (6.7.7) typedef-name:
 identifier
*/
void Parser::parseTypedefName()
{
    m_tokenSeq->expect(TokenKind::Identifier);
}

/*(6.7.8) initializer:
 assignment-expression
 { initializer-list }
 { initializer-list , }
*/
std::shared_ptr<Expr> Parser::parseInitializer()
{
    if (m_tokenSeq->match(TokenKind::LCurly_Brackets_)) {
        parseInitializerList();
        m_tokenSeq->match(TokenKind::Comma_);
        m_tokenSeq->expect(TokenKind::RCurly_Brackets_);
    }
    else {
        parseAssignExpr();
    }
    return nullptr;
}

/*(6.7.8) initializer-list:
 designationopt initializer
 initializer-list , designationopt initializer
*/
void Parser::parseInitializerList()
{
    do {
        parseDesignation();
        parseInitializer();
    } while (m_tokenSeq->match(TokenKind::Comma_));
}

/* (6.7.8) designation:
 designator-list =
*/
void Parser::parseDesignation()
{
    TokenKind tk;
    do {
        parseDesignator();
        tk = m_tokenSeq->peek()->kind_;
    }while (tk == TokenKind::LSquare_Brackets_ || tk == TokenKind::Dot_);
    m_tokenSeq->expect(TokenKind::Assign_);
}

void Parser::parseDesignator()
{
    if (m_tokenSeq->match(TokenKind::LSquare_Brackets_)) {
        parseConstansExpr();
        m_tokenSeq->expect(TokenKind::LSquare_Brackets_);
    }
    else if (m_tokenSeq->match(TokenKind::Dot_)) {
        m_tokenSeq->expect(TokenKind::Identifier);
    }
    else {
        sytaxError("expect designator, but not!");
    }
}

//---------------------------------------------------------Statements------------------------------------------------------------------------
/*6.8
 statement:
    labeled-statement
    compound-statement
    expression-statement
    selection-statement
    iteration-statement
    jump-statement
*/
std::shared_ptr<Stmt> Parser::parseStmt()
{
    switch (m_tokenSeq->peek()->kind_)
    {
    case TokenKind::Identifier:
    case TokenKind::Case:
    case TokenKind::Default:
        return parseLabeledStmt();
    
    case TokenKind::LCurly_Brackets_:
        return parseCompoundStmt();

    case TokenKind::If:
    case TokenKind::Switch:
        return parseSelectionStmt();

    case TokenKind::While:
    case TokenKind::Do:
    case TokenKind::For:
        return parseIterationStmt();

    case TokenKind::Goto:
    case TokenKind::Continue:
    case TokenKind::Break:
    case TokenKind::Return:
        return parseJumpStmt();
    
    default:
        return parseExprStmt();
    }
    return nullptr;
}

std::shared_ptr<Stmt> Parser::parseLabeledStmt()
{
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::Identifier:
    {
        // TODO
        break;
    }
    
    case TokenKind::Case:
    {
        auto node = std::make_shared<CaseStmt>();
        node->setCond(parseConstansExpr());
        m_tokenSeq->expect(TokenKind::Colon_);
        node->setBody(parseStmt());
        return node;
    }

    case TokenKind::Default:
    {
        auto node = std::make_shared<DefaultStmt>();
        m_tokenSeq->expect(TokenKind::Colon_);
        node->setBody(parseStmt());
        return node;
    }

    default:
        sytaxError("expect label, but not!");
    }
    return nullptr;
}

std::shared_ptr<CompoundStmt> Parser::parseCompoundStmt()
{
    ScopeManager scm(this, Scope::BLOCK);
    auto node = std::make_shared<CompoundStmt>();
    m_tokenSeq->expect(TokenKind::LCurly_Brackets_);
    while (!m_tokenSeq->match(TokenKind::RCurly_Brackets_)) {
        auto decls = parseDeclaration();
        for (auto decl : decls) {
            node->addStmt(std::make_shared<DeclStmt>(decl));
        }
        auto stmt = parseStmt();
        if (stmt) {
            node->addStmt(parseStmt());
        }
    }
    return node;    
}

std::shared_ptr<ExprStmt> Parser::parseExprStmt()
{
    auto expr = parseExpr();
    m_tokenSeq->expect(TokenKind::Semicolon_);
    if (expr) {
        return std::make_shared<ExprStmt>(expr);
    }
    return nullptr;
}

std::shared_ptr<Stmt> Parser::parseSelectionStmt()
{
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::If:
    {   
        m_tokenSeq->expect(TokenKind::LParent_);
        auto node = std::make_shared<IfStmt>();
        node->setCond(parseExpr());
        m_tokenSeq->expect(TokenKind::RParent_);
        node->setThen(parseStmt());
        if (m_tokenSeq->match(TokenKind::Else)) {
            node->setElse(parseStmt());
        }
        return node;
    }

    case TokenKind::Switch:
    {
        m_tokenSeq->expect(TokenKind::LParent_);
        auto node = std::make_shared<SwitchStmt>();
        node->setCond(parseExpr());
        m_tokenSeq->expect(TokenKind::RParent_);
        node->setBody(parseStmt());
        return node;
    }
    
    default:
        sytaxError("expect selection, but not!");
        return nullptr;
    }
    return nullptr;
}

/* (6.8.5) iteration-statement:
 while ( expression ) statement
 do statement while ( expression );
 for ( expressionopt ; expressionopt ; expressionopt ) statement
 for ( declaration expressionopt ; expressionopt ) statement 
*/
std::shared_ptr<Stmt> Parser::parseIterationStmt()
{
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::While:
    {
        auto node = std::make_shared<WhileStmt>();
        m_tokenSeq->expect(TokenKind::LParent_);
        node->setCond(parseExpr());
        m_tokenSeq->expect(TokenKind::RParent_);
        node->setBody(parseStmt());
        return node;
    }

    case TokenKind::Do:
    {
        auto node = std::make_shared<DoStmt>();
        node->setBody(parseStmt());
        m_tokenSeq->expect(TokenKind::While);
        m_tokenSeq->expect(TokenKind::LParent_);
        node->setCond(parseExpr());
        m_tokenSeq->expect(TokenKind::RParent_);
        m_tokenSeq->expect(TokenKind::Semicolon_);
        return node;
    }

    case TokenKind::For:
        /* code */
        break;
    
    default:
        sytaxError("expect iteration, but not!");
    }
    return nullptr;
}
/* (6.8.6) jump-statement:
 goto identifier ;
 continue ;
 break ;
 return expressionopt ;
*/
std::shared_ptr<Stmt> Parser::parseJumpStmt()
{
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::Goto:
    {
        auto node = std::make_shared<GotoStmt>();
        m_tokenSeq->expect(TokenKind::Identifier);
        auto sys = m_systable->LookupLabel(m_tokenSeq->cur()->value_);
        m_tokenSeq->expect(TokenKind::Semicolon_);
        return node;
    }

    case TokenKind::Continue:
    {
        auto node = std::make_shared<ContinueStmt>();
        m_tokenSeq->expect(TokenKind::Semicolon_);
        return node;
    }

    case TokenKind::Break:
    {
        auto node = std::make_shared<BreakStmt>();
        m_tokenSeq->expect(TokenKind::Semicolon_);
        return node;
    }

    case TokenKind::Return:
    {
        auto node = std::make_shared<ReturnStmt>();
        auto rex = parseExprStmt();
        node->setRetExpr(rex->getExpr());
        return node;
    }
    
    default:
        sytaxError("expect jump, but not!");

    }
    return nullptr;
}
//---------------------------------------------------------External definitions------------------------------------------------------------------------
/*(6.9.1) function-definition:
 declaration-specifiers declarator declaration-listopt compound-statement
 已经解析过了declaration-specifiers declarator， declaration-listopt不支持， 只解析 compound-statement
*/
Decl* Parser::parseFunctionDefinitionBody(Decl* dc)
{/*
    FunctionDecl* fd= dynamic_cast<FunctionDecl*>(dc);
    Stmt* body = parseCompoundStmt();
    fd->setBody(body);
    return fd;*/
    return nullptr;
}


/*(6.9) translation-unit:
 external-declaration
 translation-unit external-declaration
*/
void Parser::parseTranslationUnit()
{
    ScopeManager scm(this, Scope::FILE);
    m_systable->initBuiltType();
    m_unit = std::make_shared<TranslationUnitDecl>();
    while (!m_tokenSeq->match(TokenKind::EOF_))
    {
        if (m_tokenSeq->match(TokenKind::Semicolon_))
        {
            continue;
        }
        m_unit->addDecl(parseDeclaration());
    }
}
