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
Expr* Parser::parsePrimaryExpr()
{
    Expr* node = nullptr;
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::Identifier:
    {
        Symbol* sym = m_systable->lookup(Symbol::NORMAL, m_tokenSeq->cur()->value_);
        if (!sym) {
            semaError("symbol undefined!");
            return nullptr;
        }
        else if (!sym->getDecl()) {
            semaError("symbol declaration undefined!");
            return nullptr;
        }
        else {
            node = DeclRefExpr::NewObj(sym->getType()->getQualType(), sym->getDecl());
        }
        return node;
    }    

    case TokenKind::Numeric_Constant_:
        node = IntegerLiteral::NewObj(m_tokenSeq->cur());
        break;

    case TokenKind::Float_Constant:
        node = FloatingLiteral::NewObj(m_tokenSeq->cur());
        break;

    case TokenKind::Character_Constant_:
        node = CharacterLiteral::NewObj(m_tokenSeq->cur());
        break;

    case TokenKind::String_Constant_:
        node = StringLiteral::NewObj(m_tokenSeq->cur());
        break;
    
    case TokenKind::LParent_:
        node = parseExpr();
        m_tokenSeq->expect(TokenKind::RParent_);
        node = ParenExpr::NewObj(node);
        break;

    default:
        sytaxError(m_tokenSeq->cur(), "unexpect PrimaryExpr!");
        break;
    }
    return node;
}

/* (6.5.1.1) gegneric-selection:
 _Generic ( assignment-expression , generic-assoc-list
 )
 (6.5.1.1) generic-assoc-list:
 generic-association
 generic-assoc-list , generic-association
*/
Expr* Parser::parseGenericSelection()
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
Expr* Parser::parseGenericAssociation()
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
Expr* Parser::parsePostfixExpr()
{
    Expr* node = nullptr;
    if (m_tokenSeq->peek()->kind_ == TokenKind::LParent_) {
        node = parseParenExpr();
    }
    else {
        node = parsePrimaryExpr();
    }
    while (true) {
        if (m_tokenSeq->match(TokenKind::LSquare_Brackets_)) {
            auto lex = parseExpr();
            m_tokenSeq->expect(TokenKind::RSquare_Brackets_);
            node = ArraySubscriptExpr::NewObj(node, lex);
        }
        else if (m_tokenSeq->match(TokenKind::LParent_)) {
            auto lex = parseArgListExpr();
            m_tokenSeq->expect(TokenKind::RParent_);
            node = CallExpr::NewObj(node, lex);
        }
        else if (m_tokenSeq->match(TokenKind::Dot_)) {
            m_tokenSeq->expect(TokenKind::Identifier);
            Symbol* sym = m_systable->lookup(Symbol::NORMAL, m_tokenSeq->cur()->value_);
            if (!sym) {
                semaError("symbol undefined!");
                break;
            }
            QualType ty = sym->getType();
            if (ty) {
                //auto lex = DeclRefExpr::NewObj(ty, dynamic_cast<NamedDecl*>(ty->getDecl()));
                //node = MemberExpr::NewObj(node, nullptr, false);
            }
            else {
                semaError("symbol type undefined!");
                break;
            }
        }
        else if (m_tokenSeq->match(TokenKind::Arrow_)) {
            m_tokenSeq->expect(TokenKind::Identifier);
            Symbol* sym = m_systable->lookup(Symbol::NORMAL, m_tokenSeq->cur()->value_);
            if (!sym) {
                semaError("symbol undefined!");
                break;
            }
            QualType ty = sym->getType();
            if (ty) {
                //auto lex = DeclRefExpr::NewObj(ty, dynamic_cast<NamedDecl*>(ty->getDecl()));
                //node = MemberExpr::NewObj(node, nullptr, true);
            }
            else {
                semaError("symbol type undefined!");
                break;
            }
        }
        else if (m_tokenSeq->match(TokenKind::Increment_)) {
            node = UnaryOpExpr::NewObj(node, UnaryOpExpr::Post_Increment_);
        }
        else if (m_tokenSeq->match(TokenKind::Decrement_)) {
            node = UnaryOpExpr::NewObj(node, UnaryOpExpr::Post_Decrement_);
        }
        else {
            break;
        }
    }
    return node;
}

std::vector<Expr*> Parser::parseArgListExpr()
{
    std::vector<Expr*> res;
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
Expr* Parser::parseUnaryExpr()
{
    Expr* rex = nullptr;
    Expr* node = nullptr;
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::Increment_:
        rex = parseUnaryExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Pre_Increment_);
        break;

    case TokenKind::Decrement_:
        rex = parseUnaryExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Pre_Decrement_);
        break;

    case TokenKind::BitWise_AND_:
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::BitWise_AND_);
        break;

    case TokenKind::Multiplication_:
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Multiplication_);
        break;

    case TokenKind::Addition_:
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Addition_);
        break;

    case TokenKind::Subtraction_:
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Subtraction_);
        break;

    case TokenKind::BitWise_NOT_:
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::BitWise_NOT_);
        break;

    case TokenKind::Logical_NOT_:
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Logical_NOT_);
        break;
    
    case TokenKind::Sizeof:
        if (m_tokenSeq->match(TokenKind::RParent_)) {
            if (m_systable->isTypeName(m_tokenSeq->peek())) {
                parseTypeName();
                rex = ParenExpr::NewObj(nullptr);
            }
            else {
                rex = parseUnaryExpr();
            }
            m_tokenSeq->expect(TokenKind::RParent_);
        }
        else {
            rex = parseUnaryExpr();
        }
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Logical_NOT_);
        break;
    
    case TokenKind::Alignof:
        m_tokenSeq->expect(TokenKind::LParent_);
        parseTypeName();
        m_tokenSeq->expect(TokenKind::RParent_);
        break;

    default:
        node = parsePostfixExpr();
        break;
    }
    return node;
}

/*(6.5.4) cast-expression:
 unary-expression
 ( type-name ) cast-expression
*/
Expr* Parser::parseCastExpr()
{
    return nullptr;
}

Expr* Parser::parseMultiExpr()
{
    Expr* node = parseCastExpr();
    while (true) {
        if (m_tokenSeq->match(TokenKind::Multiplication_)) {
            Expr* rex = parseCastExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Multiplication_);
        } 
        else if (m_tokenSeq->match(TokenKind::Division_)) {
            Expr* rex = parseCastExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Division_);
        } 
        else if (m_tokenSeq->match(TokenKind::Modulus_)) {
            Expr* rex = parseCastExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Modulus_);
        } 
        else {
            break;
        }
    }
    return node;
}

Expr* Parser::parseAddExpr()
{
    Expr* node = parseMultiExpr();
    while (true) {
        if (m_tokenSeq->match(TokenKind::Addition_)) {
            Expr* rex = parseMultiExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Addition_);
        } 
        else if (m_tokenSeq->match(TokenKind::Subtraction_)) {
            Expr* rex = parseMultiExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Subtraction_);
        } 
        else {
            break;
        }
    }
    return node;
}

Expr* Parser::parseShiftExpr()
{
    Expr* node = parseAddExpr();
    while (true) {
        if (m_tokenSeq->match(TokenKind::LShift_)) {
            Expr* rex = parseAddExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::LShift_);
        } 
        else if (m_tokenSeq->match(TokenKind::RShift_)) {
            Expr* rex = parseAddExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::RShift_);
        } 
        else {
            break;
        }
    }
    return node;
}

Expr* Parser::parseRelationalExpr()
{
    Expr* node = parseShiftExpr();
    while (true) {
        if (m_tokenSeq->match(TokenKind::Less_)) {
            Expr* rex = parseShiftExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Less_);
        } 
        else if (m_tokenSeq->match(TokenKind::Less_Equal_)) {
            Expr* rex = parseShiftExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Less_Equal_);
        } 
        else if (m_tokenSeq->match(TokenKind::Greater_)) {
            Expr* rex = parseShiftExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Greater_);
        } 
        else if (m_tokenSeq->match(TokenKind::Greater_Equal_)) {
            Expr* rex = parseShiftExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Greater_Equal_);
        } 
        else  {
            break;
        }
    }
    return node;
}

Expr* Parser::parseEqualExpr()
{
    Expr* node = parseRelationalExpr();
    while (true) {
        if (m_tokenSeq->match(TokenKind::Equality_)) {
            Expr* rex = parseRelationalExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Equality_);
        } 
        else if (m_tokenSeq->match(TokenKind::Inequality_)) {
            Expr* rex = parseRelationalExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Inequality_);
        } 
        else {
            break;
        }
    }
    return node;
}

Expr* Parser::parseBitANDExpr()
{
    Expr* node = parseEqualExpr();
    while (m_tokenSeq->match(TokenKind::BitWise_AND_)) {
        Expr* rex = parseEqualExpr();
        node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::BitWise_AND_);
    }
    return node;
}

Expr* Parser::parseBitXORExpr()
{
    Expr* node = parseBitANDExpr();
    while (m_tokenSeq->match(TokenKind::BitWise_XOR_)) {
        Expr* rex = parseBitANDExpr();
        node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::BitWise_XOR_);
    }
    return node;
}

Expr* Parser::parseBitORExpr()
{
    Expr* node = parseBitXORExpr();
    while (m_tokenSeq->match(TokenKind::BitWise_OR_)) {
        Expr* rex = parseBitXORExpr();
        node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::BitWise_OR_);
    }
    return node;
}

Expr* Parser::parseLogicalANDExpr()
{
    Expr* node = parseBitORExpr();
    while (m_tokenSeq->match(TokenKind::Logical_AND_)) {
        Expr* rex = parseBitORExpr();
        node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Logical_AND_);
    }
    return node;
}

Expr* Parser::parseLogicalORExpr()
{
    Expr* node = parseLogicalANDExpr();
    while (m_tokenSeq->match(TokenKind::Logical_OR_)) {
        Expr* rex = parseLogicalANDExpr();
        node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Logical_OR_);
    }
    return node;
}

Expr* Parser::parseConditionalExpr()
{
    Expr* node = parseLogicalORExpr();
    if (m_tokenSeq->match(TokenKind::Conditional_)) {
        Expr* th = parseExpr();
        m_tokenSeq->expect(TokenKind::Colon_);
        Expr* el = parseConditionalExpr();
        node = ConditionalExpr::NewObj(node, th, el);
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
Expr* Parser::parseAssignExpr()
{
    Expr* rex = nullptr;
    BinaryOpExpr::OpCode opc;
    Expr* node = parseConditionalExpr();
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::Assign_:
        opc = BinaryOpExpr::Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::Mult_Assign_:
        opc = BinaryOpExpr::Mult_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::Div_Assign_:
        opc = BinaryOpExpr::Div_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::Mod_Assign_:
        opc = BinaryOpExpr::Mod_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::Add_Assign_:
        opc = BinaryOpExpr::Add_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::Sub_Assign_:
        opc = BinaryOpExpr::Sub_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::LShift_Assign_:
        opc = BinaryOpExpr::LShift_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::RShift_Assign_:
        opc = BinaryOpExpr::RShift_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::BitWise_AND_Assign_:
        opc = BinaryOpExpr::BitWise_AND_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::BitWise_XOR_Assign_:
        opc = BinaryOpExpr::BitWise_XOR_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::BitWise_OR_Assign_:
        opc = BinaryOpExpr::BitWise_OR_Assign_;
        rex = parseAssignExpr();
        break;
        
    default:
        sytaxError("expect assignment-operator, but not!");
        break;
    }
    return BinaryOpExpr::NewObj(node, rex, opc);
}

/*
 (6.5.17) expression:
 assignment-expression
 expression , assignment-expression
*/
Expr* Parser::parseExpr()
{
    Expr* node = nullptr;
    node = parseAssignExpr();
    while (m_tokenSeq->match(TokenKind::Comma_)) {
        auto rnode = parseAssignExpr();
        node = BinaryOpExpr::NewObj(node, rnode, BinaryOpExpr::Comma, QualType());
    }
    return node;
}

/*(6.6) constant-expression:
 conditional-expression
*/
Expr* Parser::parseConstansExpr()
{
    return parseConditionalExpr();
}

/* primary-expr: ( expr )
   posftix-expr： （type-name）{...}
   cast-expr: (type-name) cast-expr
*/
Expr* Parser::parseParenExpr()
{
    m_tokenSeq->expect(TokenKind::LParent_);
    Expr* node = nullptr;
    if (m_systable->isTypeName(m_tokenSeq->peek())) {
        QualType qt = parseTypeName();
        m_tokenSeq->expect(TokenKind::RParent_);
        if (m_tokenSeq->match(TokenKind::LCurly_Brackets_)) {
            //node = parseInitlizerList();
            while (m_tokenSeq->match(TokenKind::Comma_)) {
                //auto rex = parseInitlizerList();
                //node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Comma);
            }
            m_tokenSeq->expect(TokenKind::RCurly_Brackets_);
            //node = CompoundLiteralExpr::NewObj();
        }
        else {
            auto lex = parseCastExpr();
            //node = sema_->onActCastExpr(qt, lex);
        }
    }
    else {
        auto lex = parseExpr();
        m_tokenSeq->expect(TokenKind::RParent_);
        node = ParenExpr::NewObj(lex);
    }
    return node;
}

//---------------------------------------------------------Declarations------------------------------------------------------------------------
/* (6.7) declaration:
 declaration-specifiers init-declarator-listopt ;
 若类型declSpec是type-name, 则init-declarator不需要解析。
*/
DeclGroup Parser::parseDeclaration()
{
    DeclGroup res;
    int sc = 0, fs = 0;
    QualType qt = parseDeclarationSpec(&sc, &fs);
    if (m_tokenSeq->match(TokenKind::Semicolon_)) {
        if (qt->getKind() == Type::RECORD) {
            res.push_back(dynamic_cast<RecordType*>(qt.getPtr())->getTagDecl());
        }
        else if (qt->getKind() == Type::ENUM) {
            res.push_back(dynamic_cast<EnumType*>(qt.getPtr())->getTagDecl());
        }
        else {
            sytaxError("declaration specifier incomplete!");
        }
        return res;
    }

    // 解析第一个声明符
    Declarator declarator("", qt, sc, fs);
    Decl* dc = parseInitDeclarator(declarator);
    if (qt->isFunctionType() && m_tokenSeq->test(TokenKind::LCurly_Brackets_)) {
        //res.push_back(parseFunctionDefinitionBody(fstDecl));
        return res;
    } else {
        //res.push_back(fstDecl);
    }

    while (m_tokenSeq->match(TokenKind::Comma_)) {
            res.push_back(parseInitDeclarator(declarator));
    }
    m_tokenSeq->expect(TokenKind::Semicolon_);
    return res;
}

QualType Parser::parseDeclarationSpec(int* sc, int* fs)
{
    int tq = 0; // 类型限定符
    int ts = 0; // 类型说明符
    Type* ty = nullptr; // 类型：内建类型,自定义类型
    while (true)
    {
        TokenKind tkind = m_tokenSeq->next()->kind_;
        switch (tkind)
        {
        // (6.7.1) storage class specifier
        case TokenKind::Typedef:
            parseStorageClassSpec(StorageClass::TYPEDEF, sc);
            break;
        case TokenKind::Extern:
            parseStorageClassSpec(StorageClass::EXTERN, sc);
            break;
        case TokenKind::Static:
            parseStorageClassSpec(StorageClass::STATIC, sc);
            break;
        case TokenKind::Auto:
            parseStorageClassSpec(StorageClass::AUTO, sc);
            break;
        case TokenKind::Register:
            parseStorageClassSpec(StorageClass::REGISTER, sc);
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
            if (ts & ~COM_RECORD) {
                sytaxError("unexpect struct or union!");
            }
            ty = parseStructOrUnionSpec(m_tokenSeq->cur()->kind_ == TokenKind::Struct);
            return QualType(ty, tq);

        // (6.7.2) type-specifier->enum-specifier
        case TokenKind::Enum:
            if (ts & ~COM_ENUM) {
                sytaxError("unexpect enum!");
            }
            ty = parseEnumSpec();
            return QualType(ty, tq);

        //(6.7.3) type-qualifier:
        case TokenKind::Const:    tq |= TypeQualifier::CONST; break;
        case TokenKind::Volatile: tq |= TypeQualifier::VOLATILE; break;
        case TokenKind::Restrict: tq |= TypeQualifier::RESTRICT; break;

        // (6.7.4) function-specifier
        case TokenKind::Inline:
            parseFunctionSpec(FuncSpecifier::INLINE, fs);
            break;

        // (6.7.7) typedef-name 判断当前是否已有其他方式
        case TokenKind::Identifier:
            if (ts == 0) {
                if (m_systable->isTypeName(m_tokenSeq->cur())) {
                    //ty = sys_->lookup(Symbol::NORMAL, seq_->cur()->value_)->getType();
                } else {
                    sytaxError("expect type, but not!");
                }
                break;
            } else {
                ty = m_systable->getBuiltTypeByTypeSpec(ts);
                m_tokenSeq->reset();
            }

        default:
            // 遇到了标识符或者其他
            if (!ty) {
                sytaxError("incomplete type specifier!");
            }
            return QualType(ty, tq);
        }
    }
    return QualType();
}

/*init-declarator:
 declarator
 declarator = initializer
*/
Decl* Parser::parseInitDeclarator(Declarator dc)
{
    parseDeclarator(dc);
    Expr* initExpr = nullptr;
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
Type* Parser::parseStructOrUnionSpec(bool isStruct)
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
Type* Parser::parseStructDeclarationList(Symbol* sym)
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
Type* Parser::parseEnumSpec()
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
Type* Parser::parseEnumeratorList(Symbol* sym, Type* ty)
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
EnumConstantDecl* Parser::parseEnumerator(QualType qt)
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
    QualType base = parsePointer(dc.getType());
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
            m_systable->enterScope(Scope::FUNC_PROTOTYPE);
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

Expr* Parser::parseArrayLen()
{
    return nullptr;
}

QualType Parser::parseFuncOrArrayDeclarator(QualType base)
{
    if (m_tokenSeq->match(TokenKind::LSquare_Brackets_)) {
        auto len = parseArrayLen();
        m_tokenSeq->expect(TokenKind::RSquare_Brackets_);
        base = parseFuncOrArrayDeclarator(base);
        return ArrayType::NewObj(base, len);
    }
    else if (m_tokenSeq->match(TokenKind::LParent_)) {
        m_systable->enterScope(Scope::FUNC_PROTOTYPE);
        auto paramList = parseParameterList();
        m_systable->exitScope();
        m_tokenSeq->expect(TokenKind::RParent_);
        base = parseFuncOrArrayDeclarator(base);
        return FunctionType::NewObj(base, false, false, paramList);
    }
    return base;
}

/*(6.7.5) pointer:
 * type-qualifier-listopt
 * type-qualifier-listopt pointer
*/
QualType Parser::parsePointer(QualType qt)
{ 
    while (m_tokenSeq->match(TokenKind::Multiplication_)) {
        Type* ty = PointerType::NewObj(qt);
        int tq = parseTypeQualList();
        qt = QualType(ty, tq);
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
Expr* Parser::parseInitializer()
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
Stmt* Parser::parseStmt()
{
    Stmt* node = nullptr;
    switch (m_tokenSeq->peek()->kind_)
    {
    case TokenKind::Identifier:
    case TokenKind::Case:
    case TokenKind::Default:
        node = parseLabeledStmt();
        break;
    
    case TokenKind::LCurly_Brackets_:
        node = parseCompoundStmt();
        break;

    case TokenKind::If:
    case TokenKind::Switch:
        node = parseSelectionStmt();
        break;

    case TokenKind::While:
    case TokenKind::Do:
    case TokenKind::For:
        node = parseIterationStmt();
        break;

    case TokenKind::Goto:
    case TokenKind::Continue:
    case TokenKind::Break:
    case TokenKind::Return:
        node = parseJumpStmt();
        break;
    
    default:
        node = parseExprStmt();
        break;
    }
    return node;
}

Stmt* Parser::parseLabeledStmt()
{
    Stmt* node = nullptr;
    Token* tk = m_tokenSeq->next();
    switch (tk->kind_)
    {
    case TokenKind::Identifier:
    {
        LabelDecl* key = nullptr;//LabelDecl::NewObj(tk->value_);
        // 添加到符号表
        m_systable->insertLabel(tk->value_, key);
        m_tokenSeq->expect(TokenKind::Colon_);
        Stmt* body = parseStmt();
        node = LabelStmt::NewObj(key, body);
        break;
    }
    
    case TokenKind::Case:
    {
        Expr* cond = parseConstansExpr();
        m_tokenSeq->expect(TokenKind::Colon_);
        Stmt* body = parseStmt();
        node = CaseStmt::NewObj(cond, body);
        break;
    }

    case TokenKind::Default:
    {
        m_tokenSeq->expect(TokenKind::Colon_);
        Stmt* body = parseStmt();
        node = DefaultStmt::NewObj(nullptr, body);
        break;
    }

    default:
        sytaxError("expect label, but not!");
        return nullptr;
    }
    return node;
}

Stmt* Parser::parseCompoundStmt()
{
    ScopeManager scm(this, Scope::BLOCK);
    m_tokenSeq->expect(TokenKind::LCurly_Brackets_);
    std::vector<Stmt*> res;
    while (!m_tokenSeq->match(TokenKind::RCurly_Brackets_)) {
        DeclGroup dc = parseDeclaration();
        for (int i =0; i < dc.size(); i++) {
            res.push_back(DeclStmt::NewObj(dc[i]));
        }
        Stmt* st = parseStmt();
        res.push_back(st);
    }
    Stmt* node = CompoundStmt::NewObj(res);
    return node;    
}

Stmt* Parser::parseExprStmt()
{
    Expr* ex = parseExpr();
    m_tokenSeq->expect(TokenKind::Semicolon_);
    return ExprStmt::NewObj(ex);
}

Stmt* Parser::parseSelectionStmt()
{
    Stmt* node = nullptr;
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::If:
    {   
        m_tokenSeq->expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        m_tokenSeq->expect(TokenKind::RParent_);
        Stmt* th = parseStmt();
        Stmt* el = (m_tokenSeq->match(TokenKind::Else)) ? parseStmt() : nullptr;
        node = IfStmt::NewObj(cond, th, el);
        break;
    }

    case TokenKind::Switch:
    {
        m_tokenSeq->expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        m_tokenSeq->expect(TokenKind::RParent_);
        Stmt* body = parseStmt();
        node = SwitchStmt::NewObj(cond, body);
        break;
    }
    
    default:
        sytaxError("expect selection, but not!");
        return nullptr;
    }
    return node;
}

/* (6.8.5) iteration-statement:
 while ( expression ) statement
 do statement while ( expression );
 for ( expressionopt ; expressionopt ; expressionopt ) statement
 for ( declaration expressionopt ; expressionopt ) statement 
*/
Stmt* Parser::parseIterationStmt()
{
    Stmt* node = nullptr;
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::While:
    {
        m_tokenSeq->expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        m_tokenSeq->expect(TokenKind::RParent_);
        Stmt* body = parseStmt();
        node = WhileStmt::NewObj(cond, body);
        break;
    }

    case TokenKind::Do:
    {
        Stmt* body = parseStmt();
        m_tokenSeq->expect(TokenKind::While);
        m_tokenSeq->expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        m_tokenSeq->expect(TokenKind::RParent_);
        m_tokenSeq->expect(TokenKind::Semicolon_);
        return DoStmt::NewObj(cond, body);
    }

    case TokenKind::For:
        /* code */
        break;
    
    default:
        sytaxError("expect iteration, but not!");
        return nullptr;
    }
    return node;
}
/* (6.8.6) jump-statement:
 goto identifier ;
 continue ;
 break ;
 return expressionopt ;
*/
Stmt* Parser::parseJumpStmt()
{
    Stmt* node = nullptr;
    switch (m_tokenSeq->next()->kind_)
    {
    case TokenKind::Goto:
    {
        m_tokenSeq->expect(TokenKind::Identifier);
        Symbol* sym = m_systable->lookup(Symbol::LABEL, m_tokenSeq->cur()->value_);
        m_tokenSeq->expect(TokenKind::Semicolon_);
        node = GotoStmt::NewObj(nullptr);
        break;
    }

    case TokenKind::Continue:
        m_tokenSeq->expect(TokenKind::Semicolon_);
        node = ContinueStmt::NewObj(nullptr);
        break;

    case TokenKind::Break:
        m_tokenSeq->expect(TokenKind::Semicolon_);
        node = BreakStmt::NewObj(nullptr);
        break;

    case TokenKind::Return:
    {
        auto rex = parseExprStmt();
        node = ReturnStmt::NewObj(nullptr);
        break;
    }
    
    default:
        sytaxError("expect jump, but not!");
        return nullptr;

    }
    return node;
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
