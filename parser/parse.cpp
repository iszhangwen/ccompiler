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
    COM_BOOL = NONE,
    COM_VOID = NONE,
    COM_CHAR = UNSIGNED | SIGNED,
    COM_INT =  UNSIGNED | SIGNED | SHORT | LONG | LONGLONG,
    COM_FLOAT =  _COMPLEX,
    COM_DOUBLE = LONG | _COMPLEX,
    COM_COMPLEX = FLOAT | DOUBLE | LONG,
    COM_RECORD = NONE,
    COM_ENUM= NONE,
    COM_TYPEDEFNAME = NONE,
};

ScopeManager::ScopeManager(Parser* p, Scope::ScopeType st) 
: parent_(p) 
{
    parent_->sys_->enterScope(st);
}
ScopeManager::~ScopeManager() 
{
    parent_->sys_->exitScope();
}

Parser::Parser(const std::string& filename)
{
    buf_ = new Source(filename);
    seq_ = new TokenSequence(Scanner(buf_).tokenize());
    sys_ = new SymbolTableContext();
    sema_ = new SemaAnalyzer(sys_);
}

Parser:: ~Parser()
{
    delete buf_;
    delete seq_;
    delete sys_;
    delete sema_;
}

bool Parser::match(TokenKind tk)
{
    if (seq_->peek()->kind_ == tk)
    {
        seq_->next();
        return true;
    }
    return false;
}
void Parser::expect(TokenKind tk)
{
    if (seq_->peek()->kind_ == tk)
    {
        seq_->next();
        return;
    }
    std::stringstream ss;
    ss << "Expect " << Token::TokenKindMap.at(tk) << " but " << Token::TokenKindMap.at(seq_->peek()->kind_);
    sytaxError(seq_->peek(), ss.str());
}
bool Parser::test(TokenKind tk)
{
    return seq_->peek()->kind_ == tk;
}
void Parser::reset()
{
    seq_->reset();
}

void Parser::sytaxError(const std::string& val)
{
    sytaxError(seq_->cur(), val);
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
        << buf_->segline(tk->loc_)
        << std::string(tk->loc_.column, ' ') 
        << "^ "  
        << RED 
        << std::string(buf_->segline(tk->loc_).size() - tk->loc_.column, '~') 
        << CANCEL 
        << std::endl;
    #undef RED
    #undef CANCEL
    throw CCError(ss.str());
}

void Parser::semaError(const std::string& val)
{
    semaError(seq_->cur(), val);
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
        << buf_->segline(tk->loc_)
        << std::string(tk->loc_.column, ' ') 
        << "^ "  
        << RED 
        << std::string(buf_->segline(tk->loc_).size() - tk->loc_.column - 1, '~') 
        << CANCEL 
        << std::endl;
    #undef RED
    #undef CANCEL
    throw CCError(ss.str());
}
void Parser::dumplog(std::string val = "")
{
    Token* tk = seq_->cur();
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
        << "log: " 
        << CANCEL
        << val
        << std::endl
        << buf_->segline(tk->loc_)
        << std::string(tk->loc_.column, ' ') 
        << "^ "  
        << RED 
        << std::string(buf_->segline(tk->loc_).size() - tk->loc_.column, '~') 
        << CANCEL 
        << std::endl;
    #undef RED
    #undef CANCEL
    std::cout << ss.str();
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
    switch (seq_->peek()->kind_)
    {
    case TokenKind::Identifier:
    {
        seq_->next();
        Symbol* sym = sys_->lookup(Symbol::NORMAL, seq_->cur()->value_);
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
        seq_->next();
        node = IntegerLiteral::NewObj(seq_->cur());
        break;

    case TokenKind::Float_Constant:
        seq_->next();
        node = FloatingLiteral::NewObj(seq_->cur());
        break;

    case TokenKind::Character_Constant_:
        seq_->next();
        node = CharacterLiteral::NewObj(seq_->cur());
        break;

    case TokenKind::String_Constant_:
        seq_->next();
        node = StringLiteral::NewObj(seq_->cur());
        break;
    
    case TokenKind::LParent_:
        seq_->next();
        node = parseExpr();
        expect(TokenKind::RParent_);
        node = ParenExpr::NewObj(node);
        break;

    default:
        sytaxError(seq_->cur(), "unexpect PrimaryExpr!");
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
    expect(TokenKind::T_Generic);
    expect(TokenKind::LParent_);
    parseAssignExpr();
    while (match(TokenKind::Comma_)) {
        parseGenericAssociation();
    }
    expect(TokenKind::RParent_);
    return nullptr;
}

/*(6.5.1.1) generic-association:
 type-name : assignment-expression
 default : assignment-expression
*/
Expr* Parser::parseGenericAssociation()
{
    if (match(TokenKind::Default)) {

    }
    else {
        parseTypeName();
    }
    match(TokenKind::Colon_);
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
    if (seq_->peek()->kind_ == TokenKind::LParent_) {
        node = parseParenExpr();
    }
    else {
        node = parsePrimaryExpr();
    }
    while (true) {
        if (match(TokenKind::LSquare_Brackets_)) {
            auto lex = parseExpr();
            expect(TokenKind::RSquare_Brackets_);
            node = ArraySubscriptExpr::NewObj(node, lex);
        }
        else if (match(TokenKind::LParent_)) {
            auto lex = parseArgListExpr();
            expect(TokenKind::RParent_);
            node = CallExpr::NewObj(node, lex);
        }
        else if (match(TokenKind::Dot_)) {
            expect(TokenKind::Identifier);
            Symbol* sym = sys_->lookup(Symbol::NORMAL, seq_->cur()->value_);
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
        else if (match(TokenKind::Arrow_)) {
            expect(TokenKind::Identifier);
            Symbol* sym = sys_->lookup(Symbol::NORMAL, seq_->cur()->value_);
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
        else if (match(TokenKind::Increment_)) {
            node = UnaryOpExpr::NewObj(node, UnaryOpExpr::Post_Increment_);
        }
        else if (match(TokenKind::Decrement_)) {
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
    while (match(TokenKind::Comma_)) {
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
    switch (seq_->peek()->kind_)
    {
    case TokenKind::Increment_:
        seq_->next();
        rex = parseUnaryExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Pre_Increment_);
        break;

    case TokenKind::Decrement_:
        seq_->next();
        rex = parseUnaryExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Pre_Decrement_);
        break;

    case TokenKind::BitWise_AND_:
        seq_->next();
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::BitWise_AND_);
        break;

    case TokenKind::Multiplication_:
        seq_->next();
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Multiplication_);
        break;

    case TokenKind::Addition_:
        seq_->next();
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Addition_);
        break;

    case TokenKind::Subtraction_:
        seq_->next();
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Subtraction_);
        break;

    case TokenKind::BitWise_NOT_:
        seq_->next();
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::BitWise_NOT_);
        break;

    case TokenKind::Logical_NOT_:
        seq_->next();
        rex = parseCastExpr();
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Logical_NOT_);
        break;
    
    case TokenKind::Sizeof:
        seq_->next();
        if (match(TokenKind::RParent_)) {
            if (sys_->isTypeName(seq_->peek())) {
                parseTypeName();
                rex = ParenExpr::NewObj(nullptr);
            }
            else {
                rex = parseUnaryExpr();
            }
            expect(TokenKind::RParent_);
        }
        else {
            rex = parseUnaryExpr();
        }
        node = UnaryOpExpr::NewObj(rex, UnaryOpExpr::Logical_NOT_);
        break;
    
    case TokenKind::Alignof:
        seq_->next();
        expect(TokenKind::LParent_);
        parseTypeName();
        expect(TokenKind::RParent_);
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
    Expr* node = nullptr;
    switch (seq_->peek()->kind_)
    {
    case TokenKind::LParent_:
        node = parseParenExpr();
        break;
    
    default:
        node = parseUnaryExpr();
        break;
    }
    return node;
}

Expr* Parser::parseMultiExpr()
{
    Expr* node = parseCastExpr();
    while (true) {
        if (match(TokenKind::Multiplication_)) {
            Expr* rex = parseCastExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Multiplication_);
        } 
        else if (match(TokenKind::Division_)) {
            Expr* rex = parseCastExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Division_);
        } 
        else if (match(TokenKind::Modulus_)) {
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
        if (match(TokenKind::Addition_)) {
            Expr* rex = parseMultiExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Addition_);
        } 
        else if (match(TokenKind::Subtraction_)) {
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
        if (match(TokenKind::LShift_)) {
            Expr* rex = parseAddExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::LShift_);
        } 
        else if (match(TokenKind::RShift_)) {
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
        if (match(TokenKind::Less_)) {
            Expr* rex = parseShiftExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Less_);
        } 
        else if (match(TokenKind::Less_Equal_)) {
            Expr* rex = parseShiftExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Less_Equal_);
        } 
        else if (match(TokenKind::Greater_)) {
            Expr* rex = parseShiftExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Greater_);
        } 
        else if (match(TokenKind::Greater_Equal_)) {
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
        if (match(TokenKind::Equality_)) {
            Expr* rex = parseRelationalExpr();
            node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Equality_);
        } 
        else if (match(TokenKind::Inequality_)) {
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
    while (match(TokenKind::BitWise_AND_)) {
        Expr* rex = parseEqualExpr();
        node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::BitWise_AND_);
    }
    return node;
}

Expr* Parser::parseBitXORExpr()
{
    Expr* node = parseBitANDExpr();
    while (match(TokenKind::BitWise_XOR_)) {
        Expr* rex = parseBitANDExpr();
        node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::BitWise_XOR_);
    }
    return node;
}

Expr* Parser::parseBitORExpr()
{
    Expr* node = parseBitXORExpr();
    while (match(TokenKind::BitWise_OR_)) {
        Expr* rex = parseBitXORExpr();
        node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::BitWise_OR_);
    }
    return node;
}

Expr* Parser::parseLogicalANDExpr()
{
    Expr* node = parseBitORExpr();
    while (match(TokenKind::Logical_AND_)) {
        Expr* rex = parseBitORExpr();
        node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Logical_AND_);
    }
    return node;
}

Expr* Parser::parseLogicalORExpr()
{
    Expr* node = parseLogicalANDExpr();
    while (match(TokenKind::Logical_OR_)) {
        Expr* rex = parseLogicalANDExpr();
        node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Logical_OR_);
    }
    return node;
}

Expr* Parser::parseConditionalExpr()
{
    Expr* node = parseLogicalORExpr();
    if (match(TokenKind::Conditional_)) {
        Expr* th = parseExpr();
        expect(TokenKind::Colon_);
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
    switch (seq_->peek()->kind_)
    {
    case TokenKind::Assign_:
        seq_->next(); // consume '='
        opc = BinaryOpExpr::Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::Mult_Assign_:
        seq_->next(); // consume '*='
        opc = BinaryOpExpr::Mult_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::Div_Assign_:
        seq_->next(); // consume '/='
        opc = BinaryOpExpr::Div_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::Mod_Assign_:
        seq_->next(); // consume '%='
        opc = BinaryOpExpr::Mod_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::Add_Assign_:
        seq_->next(); // consume '+='
        opc = BinaryOpExpr::Add_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::Sub_Assign_:
        seq_->next(); // consume '-='
        opc = BinaryOpExpr::Sub_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::LShift_Assign_:
        seq_->next(); // consume '<<='
        opc = BinaryOpExpr::LShift_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::RShift_Assign_:
        seq_->next(); // consume '>>='
        opc = BinaryOpExpr::RShift_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::BitWise_AND_Assign_:
        seq_->next(); // consume '&='
        opc = BinaryOpExpr::BitWise_AND_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::BitWise_XOR_Assign_:
        seq_->next(); // consume '^='
        opc = BinaryOpExpr::BitWise_XOR_Assign_;
        rex = parseAssignExpr();
        break;

    case TokenKind::BitWise_OR_Assign_:
        seq_->next(); // consume '|='
        opc = BinaryOpExpr::BitWise_OR_Assign_;
        rex = parseAssignExpr();
        break;
        
    default:
        return node;
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
    while (match(TokenKind::Comma_)) {
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
    expect(TokenKind::LParent_);    
    Expr* node = nullptr;
    if (sys_->isTypeName(seq_->peek())) {
        QualType qt = parseTypeName();
        expect(TokenKind::RParent_);
        if (match(TokenKind::LCurly_Brackets_)) {
            //node = parseInitlizerList();
            while (match(TokenKind::Comma_)) {
                //auto rex = parseInitlizerList();
                //node = BinaryOpExpr::NewObj(node, rex, BinaryOpExpr::Comma);
            }
            expect(TokenKind::RCurly_Brackets_);
            //node = CompoundLiteralExpr::NewObj();
        }
        else {
            auto lex = parseCastExpr();
            //node = sema_->onActCastExpr(qt, lex);
        }
    }
    else {
        auto lex = parseExpr();
        expect(TokenKind::RParent_);
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
    // 未解析到类型
    if (qt.isNull()){
        sytaxError("has not declaration-specifiers!");
        return res;
    }

    // 如果是struct union enum定义则返回。
    if (match(TokenKind::Semicolon_)) {
        switch (qt->getKind())
        {
        case Type::STRUCT:
        case Type::UNION:
       {
            RecordType* ty = dynamic_cast<RecordType*>(qt.getPtr());
            if (ty->getTagDecl()) {
                res.push_back(ty->getTagDecl());
            }
            break;
        }
        case Type::ENUM:
        {
            EnumType* ty = dynamic_cast<EnumType*>(qt.getPtr());
            if (ty->getTagDecl()) {
                res.push_back(ty->getTagDecl());
            }
            break;
        }
        default:
            sytaxError("declaration specifier incomplete!");
            break;
        }
        return res;
    }

    // 解析第一个声明符
    Declarator declarator(nullptr, qt, sc, fs);
    Decl* dc = parseInitDeclarator(declarator);
    res.push_back(dc);
    // 解析函数定义
    if (dc->isFunctionDecl() && test(TokenKind::LCurly_Brackets_)) {
        FunctionDecl* fc = dynamic_cast<FunctionDecl*>(dc);
        parseFunctionDefinitionBody(fc);
        return res;
    } 
    while (match(TokenKind::Comma_)) {
        res.push_back(parseInitDeclarator(declarator));
    }
    expect(TokenKind::Semicolon_);
    return res;
}

QualType Parser::parseDeclarationSpec(int* sc, int* fs)
{
    int tq = 0; // 类型限定符
    int ts = 0; // 类型说明符
    QualType ty; // 类型：内建类型,自定义类型
    while (true)
    {
        switch (seq_->peek()->kind_)
        {
        // (6.7.1) storage class specifier
        case TokenKind::Typedef:
            seq_->next(); 
            parseStorageClassSpec(StorageClass::TYPEDEF, sc);
            break;
        case TokenKind::Extern:
            seq_->next(); 
            parseStorageClassSpec(StorageClass::EXTERN, sc);
            break;
        case TokenKind::Static:
            seq_->next(); 
            parseStorageClassSpec(StorageClass::STATIC, sc);
            break;
        case TokenKind::Auto:
            seq_->next(); 
            parseStorageClassSpec(StorageClass::AUTO, sc);
            break;
        case TokenKind::Register:
            seq_->next(); 
            parseStorageClassSpec(StorageClass::REGISTER, sc);
            break;

        // (6.7.2) type specifiers
        // 使用状态机解析TypeSpecifier
        case TokenKind::T_Bool:
            seq_->next();
            if (ts & ~COM_BOOL) {
                sytaxError("unexpect bool!");
            }
            ts |= TypeSpecifier::_BOOL;
            break;

        case TokenKind::T_Complex:
            seq_->next();
            if (ts & ~COM_COMPLEX) {
                sytaxError("unexpect complex!");
            }
            ts |= TypeSpecifier::_COMPLEX;
            break;

        case TokenKind::Void:
            seq_->next();
            if (ts & ~COM_VOID) {
                sytaxError("unexpect void!");
            }
            ts |= TypeSpecifier::VOID;
            break;

        case TokenKind::Char:
            seq_->next();
            if (ts & ~COM_CHAR) {
                sytaxError("unexpect char!");
            }
            ts |= TypeSpecifier::CHAR;
            break;

        case TokenKind::Int:
            seq_->next();
            if (ts & ~COM_INT) {
                sytaxError("unexpect int!");
            }
            ts |= TypeSpecifier::INT;
            break;

        case TokenKind::Float:
            seq_->next();
            if (ts & ~COM_FLOAT) {
                sytaxError("unexpect float!");
            }
            ts |= TypeSpecifier::FLOAT;
            break;

        case TokenKind::Double:
            seq_->next();
            if (ts & ~COM_DOUBLE) {
                sytaxError("unexpect double!");
            }
            ts |= TypeSpecifier::DOUBLE;
            break;

        case TokenKind::Signed:
            seq_->next();
            if (ts & ~COM_SIGNED) {
                sytaxError("unexpect signed!");
            }
            ts |= TypeSpecifier::SIGNED;
            break;

        case TokenKind::Unsigned:
            seq_->next();
            if (ts & ~COM_UNSIGNED) {
                sytaxError("unexpect unsigned!");
            }
            ts |= TypeSpecifier::UNSIGNED;
            break;

        case TokenKind::Short:
            seq_->next();
            if (ts & ~COM_SHORT) {
                sytaxError("unexpect short!");
            }
            ts |= TypeSpecifier::SHORT;
            break;

        case TokenKind::Long:
            seq_->next();
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
            seq_->next();
            if (ts & ~COM_RECORD) {
                sytaxError("unexpect struct or union!");
            }
            return parseStructOrUnionSpec(seq_->cur()->kind_ == TokenKind::Struct);

        // (6.7.2) type-specifier->enum-specifier
        case TokenKind::Enum:
            seq_->next();
            if (ts & ~COM_ENUM) {
                sytaxError("unexpect enum!");
            }
            return parseEnumSpec();

        //(6.7.3) type-qualifier:
        case TokenKind::Const:    seq_->next(); tq |= TypeQualifier::CONST; break;
        case TokenKind::Volatile: seq_->next(); tq |= TypeQualifier::VOLATILE; break;
        case TokenKind::Restrict: seq_->next(); tq |= TypeQualifier::RESTRICT; break;

        // (6.7.4) function-specifier
        case TokenKind::Inline:
            seq_->next();
            parseFunctionSpec(FuncSpecifier::INLINE, fs);
            break;

        // (6.7.7) typedef-name 判断当前是否已有其他方式
        case TokenKind::Identifier:
            if (ts == 0) {
                if (sys_->isTypeName(seq_->next())) {
                    Symbol* sym = sys_->lookup(Symbol::NORMAL, seq_->cur()->value_);
                    ty = sym ? sym->getType() : QualType(nullptr);
                } else {
                    sytaxError("expect type, but not!");
                }
                break;
            } else {
                ty = sys_->getBuiltTypeByTypeSpec(ts);
            }

        default:
            // 遇到了标识符或者其他
            return ty;
        }
    }
    return ty;
}

/*init-declarator:
 declarator
 declarator = initializer
*/
Decl* Parser::parseInitDeclarator(Declarator dc)
{
    parseDeclarator(dc);
    Expr* initExpr = nullptr;
    if (match(TokenKind::Assign_)) {
        initExpr = parseInitializer();
    }
    // 根据Declarator生成Decl
    // 判断是否是typedefined
    if (dc.getStorageClass() & StorageClass::TYPEDEF) {
        if (initExpr) {
            sytaxError("typedef can not have initializer!");
        }
        TypedefDecl* res = nullptr;
        if (sys_->lookup(Symbol::NORMAL, dc.getName())) {
            semaError("has defined typedef!");
        } else {
            res = new TypedefDecl(dc.getName(), sys_->getCurScope(), dc.getType());
            sys_->insertNormal(dc.getName(), dc.getType(), res, true);
        }
        
        return res;
    }
    // 判断是否是函数声明
    if (dc.getType() && dc.getType()->isFunctionType()) {
        if (initExpr) {
            sytaxError("function can not have initializer!");
        }
        Symbol* sym = sys_->lookup(Symbol::NORMAL, dc.getName());
        if (sym) {
            // 已经声明了函数，类型要相同 TODO

        }
        FunctionDecl* res = new FunctionDecl(dc.getName(), sys_->getCurScope(), dc.getType(), dc.getStorageClass(), dc.getFuncSpec());
        sys_->insertNormal(dc.getName(), dc.getType(), res, true);
        return res;
    }
    // 若既不是函数声明，也不是typedef，则是变量声明
    VarDecl* res = nullptr;
    if (sys_->lookup(Symbol::NORMAL, dc.getName())) {
        semaError(dc.getToken(), "has defined var!");
    } else {
        res = new VarDecl(dc.getName(), sys_->getCurScope(), dc.getType(), dc.getStorageClass(), initExpr);
        sys_->insertNormal(dc.getName(), dc.getType(), res);
    }
    return res;

}

void Parser::parseStorageClassSpec(StorageClass val, int* sc)
{
    if (sc == nullptr) {
        sytaxError("expect not storageclass, but has!");
        return;
    } 
    else if (*sc != 0) {
        sytaxError("duplication storageclass!");
        return;
    }
    *sc |= val;
}

/* (6.7.2.1) struct-or-union-specifier:
 struct-or-union identifieropt { struct-declaration-list }
 struct-or-union identifier
*/
QualType Parser::parseStructOrUnionSpec(bool isUnion)
{
    //符号解析
    std::string key;
    if (match(TokenKind::Identifier)) {        
        key = seq_->cur()->value_;
    }
    Symbol* sym = sys_->lookup(Symbol::RECORD, key);
    // UDT定义
    if (match(TokenKind::LCurly_Brackets_)) {
        // 符号表没查找到:第一次定义
        if (!sym) {
            if (!key.empty()) { // 匿名对象不插入符号表
                QualType ty = QualType(RecordType::NewObj(nullptr, isUnion));
                sym = sys_->insertRecord(key, ty, nullptr);
            }
            return parseStructDeclarationList(sym, isUnion);
        }
        // 符号表查找到了但是类型定义不完整：存在前向声明
        else if (!sym->getType()->isCompleteType()) {
            return parseStructDeclarationList(sym, isUnion);
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
        return sym->getType();
    }
    QualType ty = QualType(RecordType::NewObj(nullptr, isUnion));
    sys_->insertRecord(key, ty, nullptr);
    return ty;
}

/* (6.7.2.1) struct-declaration-list:
 struct-declaration
 struct-declaration-list struct-declaration
 struct-declaration:
 specifier-qualifier-list struct-declarator-list ;
 解析结构体成员
*/
QualType Parser::parseStructDeclarationList(Symbol* sym, bool isUnion)
{
    QualType ty;
    std::string name;
    if (sym) {
        ty = sym->getType();
        name = sym->getKey();
    } else {
        ty = QualType(RecordType::NewObj(nullptr, isUnion), 0);
    }
    RecordDecl* dc = new RecordDecl(name, sys_->getCurScope(), ty, true, isUnion);
    ScopeManager scm(this, Scope::BLOCK);
    do {
        auto path = parseStructDeclaratorList(parseSpecQualList(), dc);
        expect(TokenKind::Semicolon_);
        dc->addField(path);
    }while (!match(TokenKind::RCurly_Brackets_));
    if (ty.getPtr()) {
        dynamic_cast<RecordType*>(ty.getPtr())->setTagDecl(dc);
    }
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
std::vector<FieldDecl*> Parser::parseStructDeclaratorList(QualType qt, RecordDecl* parent)
{
    std::vector<FieldDecl*> res;
    do {
        Expr* initEx = nullptr;
        Declarator declaraor(nullptr, qt, 0, 0);
        if (match(TokenKind::Colon_)) {
            initEx = parseConstansExpr();
        }
        else {
            parseDeclarator(declaraor);
            if (match(TokenKind::Colon_)) {

                initEx = parseConstansExpr();
            }
        }
        res.push_back(new FieldDecl(declaraor.getName(), sys_->getCurScope(), qt, parent, initEx));

    }while (match(TokenKind::Comma_));
    return res;
}

/* (6.7.2.2) enum-specifier:
 enum identifieropt { enumerator-list }
 enum identifieropt { enumerator-list ,}
 enum identifier
*/
QualType Parser::parseEnumSpec()
{
    // 符号解析
    std::string key;
    if (match(TokenKind::Identifier)) {
        key = seq_->cur()->value_;
    }

    // 枚举定义解析
    Symbol* sym = sys_->lookup(Symbol::RECORD, key);
    if (match(TokenKind::LCurly_Brackets_)) {
        // 符号表没有查找到，第一次定义
        if (!sym) {
            if (!key.empty()) { // 匿名对象不插入符号表
                QualType ty = QualType(EnumType::NewObj(nullptr));
                sys_->insertRecord(key, ty, nullptr);
            }
            return parseEnumeratorList(sym);
        }
        // 符号表查找到了但是未定义
        else if (!sym->getType()->isCompleteType()) {
            return parseEnumeratorList(sym);
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
    if (sym) {
        return sym->getType();
    }
    QualType ty = QualType(EnumType::NewObj(nullptr));
    sys_->insertRecord(key, ty, nullptr);
    return ty;
}

/*  (6.7.2.2) enumerator-list:
 enumerator
 enumerator-list , enumerator
*/
QualType Parser::parseEnumeratorList(Symbol* sym)
{
    // 打开块作用域
    std::string name;
    QualType ty;
    if (sym) {
        name = sym->getKey();
        ty = sym->getType();
    } else {
        ty = EnumType::NewObj(nullptr);
    }
    EnumDecl* dc = new EnumDecl(name, sys_->getCurScope(), ty, true);
    ScopeManager scm(this, Scope::BLOCK);
    while (true) {
        auto child = parseEnumerator(QualType(), dc);
        dc->addConstant(child);
        // 匹配到逗号
        if (match(TokenKind::Comma_)) {
            if (match(TokenKind::RCurly_Brackets_)) {
                break;
            }
            continue;
        }
        // 未匹配到逗号，则必定结束
        else {
            expect(TokenKind::RCurly_Brackets_);
            break;
        }
    }
    // 匿名对象则创建类型
    if (!ty.isNull()) {
        dynamic_cast<EnumType*>(ty.getPtr())->setTagDecl(dc);
    }
    return ty;
}

/*(6.7.2.2) enumerator:
 enumeration-constant
 enumeration-constant = constant-expression
  (6.4.4.3) enumeration-constant:
 identifier
*/
EnumConstantDecl* Parser::parseEnumerator(QualType qt, EnumDecl* parent)
{
    // 解析符号
    expect(TokenKind::Identifier);
    std::string name = seq_->cur()->value_;
    // 解析表达式
    Expr* ex = nullptr;
    if (match(TokenKind::Assign_)) {
        ex = parseConstansExpr();
    }
    EnumConstantDecl* dc = new EnumConstantDecl(name, sys_->getCurScope(), qt, parent, ex);
    // 插入符号表
    sys_->insertMember(name, qt, dc);
    return dc;
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
        switch (seq_->peek()->kind_)
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
        seq_->next();
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
 (6.7.6) direct-declarator:
 identifier
 ( declarator )
 direct-declarator [ type-qualifier-listopt assignment-expressionopt ]
 direct-declarator [ static type-qualifier-listopt assignment-expression ]
 direct-declarator [ type-qualifier-list static assignment-expression ]
 direct-declarator [ type-qualifier-listopt *]
 direct-declarator ( parameter-type-list )
 direct-declarator ( identifier-listopt )
*/
void Parser::parseDeclarator(Declarator& dc)
{
    // 计算pointer指针类型
    QualType base = parsePointer(dc.getType());
    dc.setType(base);
    // （ declarator ）
    if (match(TokenKind::LParent_)) {
        parseDeclarator(dc);
        expect(TokenKind::RParent_);
        QualType newBase = parseFuncOrArrayDeclarator(base);
        // 修正符号类型
        // 例：int* (*a)[23]; base: int*, ret.second: int**, newBase: int* [23]
        // 实际类型是派生自int*[23]的指针类型
        dc.setType(modifyBaseType(base, newBase, dc.getType()));
    }
    else if (match(TokenKind::Identifier)) {
        dc.setToken(seq_->cur());
        dc.setType(parseFuncOrArrayDeclarator(base));
    }
    else {
        dc.setType(parseFuncOrArrayDeclarator(base));
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

/*
 direct-declarator [ type-qualifier-listopt assignment-expressionopt ]
 direct-declarator [ static type-qualifier-listopt assignment-expression ]
 direct-declarator [ type-qualifier-list static assignment-expression ]
 direct-declarator [ type-qualifier-listopt *]
*/
Expr* Parser::parseArrayLen()
{
    return parseAssignExpr();
}

QualType Parser::parseFuncOrArrayDeclarator(QualType base)
{
    if (match(TokenKind::LSquare_Brackets_)) {
        auto len = parseArrayLen();
        expect(TokenKind::RSquare_Brackets_);
        base = parseFuncOrArrayDeclarator(base);
        return ArrayType::NewObj(base, len);
    }
    else if (match(TokenKind::LParent_)) {
        // 声明阶段函数参数不加入到原型作用域，定义时才加入
        auto paramList = parseParameterList();
        expect(TokenKind::RParent_);
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
    while (match(TokenKind::Multiplication_)) {
        Type* ty = PointerType::NewObj(qt);
        int tq = parseTypeQualList();
        qt = QualType(ty, tq);
    }
    return qt;
}

void Parser::parseParameterTypeList()
{
    parseParameterList();
    if (match(TokenKind::Comma_)) {
        expect(TokenKind::Ellipsis_);
    }
}

/* (6.7.6) parameter-list:
 parameter-declaration
 parameter-list , parameter-declaration
*/
std::vector<ParmVarDecl*> Parser::parseParameterList()
{
    std::vector<ParmVarDecl*> res;
    // 解析第一个参数
    if (test(TokenKind::RParent_)) {
        return res;
    }
    res.push_back(parseParameterDeclaration());
    while (match(TokenKind::Comma_)) {
        res.push_back(parseParameterDeclaration());
    }
    return res;
}

/* (6.7.5) parameter-declaration:
 declaration-specifiers declarator
 declaration-specifiers abstract-declaratoropt
*/
ParmVarDecl* Parser::parseParameterDeclaration()
{
    QualType qt = parseDeclarationSpec(nullptr, nullptr);
    if (qt.isNull()) {
        return nullptr;
    }
    Declarator dc(nullptr, qt, 0, 0);
    // 首先解析declarator
    parseDeclarator(dc);
    // 有名字的参数
    ParmVarDecl* param = new ParmVarDecl(dc.getName(), sys_->getCurScope(), dc.getType(), dc.getStorageClass());
    return param;
}

/* (6.7.5) identifier-list:
 identifier
 identifier-list , identifier
*/
void Parser::parseIdentifierList()
{
    expect(TokenKind::Identifier);
    while (match(TokenKind::Comma_)) {
        expect(TokenKind::Identifier);
    }

}

/*(6.7.6) type-name:
 specifier-qualifier-list abstract-declaratoropt
*/
QualType Parser::parseTypeName()
{
    return QualType();
}

/* (6.7.6) abstract-declarator:
 pointer
 pointeropt direct-abstract-declarator
 (6.7.7) direct-abstract-declarator:
 ( abstract-declarator )
 direct-abstract-declaratoropt [ type-qualifier-listopt assignment-expressionopt ]
 direct-abstract-declaratoropt [ static type-qualifier-listopt assignment-expression ]
 direct-abstract-declaratoropt [ type-qualifier-list static assignment-expression ]
 direct-abstract-declaratoropt [ * ]
 direct-abstract-declaratoropt ( parameter-type-listopt )
*/
void Parser::parseAbstractDeclarator(Declarator& dc)
{

}


/* (6.7.7) typedef-name:
 identifier
*/
void Parser::parseTypedefName()
{
    expect(TokenKind::Identifier);
}

/*(6.7.8) initializer:
 assignment-expression
 { initializer-list }
 { initializer-list , }
*/
Expr* Parser::parseInitializer()
{
    Expr* node = nullptr;
    if (match(TokenKind::LCurly_Brackets_)) {
        parseInitializerList();
        match(TokenKind::Comma_);
        expect(TokenKind::RCurly_Brackets_);
    }
    else {
        node = parseAssignExpr();
    }
    return node;
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
    } while (match(TokenKind::Comma_));
}

/* (6.7.8) designation:
 designator-list =
*/
void Parser::parseDesignation()
{
    TokenKind tk;
    do {
        parseDesignator();
        tk = seq_->peek()->kind_;
    }while (tk == TokenKind::LSquare_Brackets_ || tk == TokenKind::Dot_);
    expect(TokenKind::Assign_);
}

void Parser::parseDesignator()
{
    if (match(TokenKind::LSquare_Brackets_)) {
        parseConstansExpr();
        expect(TokenKind::LSquare_Brackets_);
    }
    else if (match(TokenKind::Dot_)) {
        expect(TokenKind::Identifier);
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
    switch (seq_->peek()->kind_)
    {
    case TokenKind::Identifier:
        if (seq_->peek(2)->kind_ != TokenKind::Colon_) {
            node = parseExprStmt();
            break;
        } 
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
    switch (seq_->peek()->kind_)
    {
    case TokenKind::Identifier:
    {
        seq_->next();
        LabelDecl* key = new LabelDecl(seq_->cur()->value_, sys_->getCurScope());
        // 添加到符号表
        sys_->insertLabel(seq_->cur()->value_, key);
        expect(TokenKind::Colon_);
        Stmt* body = parseStmt();
        node = new LabelStmt(key, body);
        break;
    }
    
    case TokenKind::Case:
    {
        seq_->next();
        Expr* cond = parseConstansExpr();
        expect(TokenKind::Colon_);
        Stmt* body = parseStmt();
        node = new CaseStmt(cond, body);
        break;
    }

    case TokenKind::Default:
    {
        seq_->next();
        expect(TokenKind::Colon_);
        Stmt* body = parseStmt();
        node = new DefaultStmt(nullptr, body);
        break;
    }

    default:
        sytaxError("expect label, but not!");
        return nullptr;
    }
    return node;
}

/* (6.8.2) compound-statement:
 { block-item-listopt }
 (6.8.2) block-item-list:
 block-item
 block-item-list block-item
 (6.8.2) block-item:
 declaration
 statement
*/
CompoundStmt* Parser::parseCompoundStmt(bool isFunc)
{
    ScopeManager scm(this, isFunc? Scope::FUNC : Scope::BLOCK);
    expect(TokenKind::LCurly_Brackets_);
    std::vector<Stmt*> res;
    while (!match(TokenKind::RCurly_Brackets_)) {
        // 解析声明
        if (sys_->isDeclarationSpecifier(seq_->peek())) {
            DeclGroup dcs = parseDeclaration();
            for (auto dc : dcs) {
                if (dc) {
                    res.push_back(new DeclStmt(dc));
                }
            }
        }
        // 解析语句 
        else {
            Stmt* st = parseStmt();
            if (st) {
                res.push_back(st);
            }
        }
    }
    return new CompoundStmt(res);
}

/* (6.8.3) expression-statement:
 expressionopt ;
*/
ExprStmt* Parser::parseExprStmt()
{
    Expr* ex = nullptr;
    if (!match(TokenKind::Semicolon_)) {
        ex = parseExpr();
        expect(TokenKind::Semicolon_);
    }
    return new ExprStmt(ex);
}

Stmt* Parser::parseSelectionStmt()
{
    Stmt* node = nullptr;
    switch (seq_->peek()->kind_)
    {
    case TokenKind::If:
    {   
        seq_->next();
        expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        expect(TokenKind::RParent_);
        Stmt* th = parseStmt();
        Stmt* el = (match(TokenKind::Else)) ? parseStmt() : nullptr;
        node = IfStmt::NewObj(cond, th, el);
        break;
    }

    case TokenKind::Switch:
    {
        seq_->next();
        expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        expect(TokenKind::RParent_);
        Stmt* body = parseStmt();
        node = new SwitchStmt(cond, body);
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
    switch (seq_->next()->kind_)
    {
    case TokenKind::While:
    {
        expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        expect(TokenKind::RParent_);
        Stmt* body = parseStmt();
        node = WhileStmt::NewObj(cond, body);
        break;
    }

    case TokenKind::Do:
    {
        Stmt* body = parseStmt();
        expect(TokenKind::While);
        expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        expect(TokenKind::RParent_);
        expect(TokenKind::Semicolon_);
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
    switch (seq_->next()->kind_)
    {
    case TokenKind::Goto:
    {
        expect(TokenKind::Identifier);
        Symbol* sym = sys_->lookup(Symbol::LABEL, seq_->cur()->value_);
        expect(TokenKind::Semicolon_);
        node = new GotoStmt(nullptr);
        break;
    }

    case TokenKind::Continue:
        expect(TokenKind::Semicolon_);
        node = ContinueStmt::NewObj(nullptr);
        break;

    case TokenKind::Break:
        expect(TokenKind::Semicolon_);
        node = BreakStmt::NewObj(nullptr);
        break;

    case TokenKind::Return:
    {
        auto rex = parseExprStmt();
        node = new ReturnStmt(rex);
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
void Parser::parseFunctionDefinitionBody(FunctionDecl* dc)
{
    // 打开函数原型作用域，将参数添加到函数声明中
    ScopeManager scm(this, Scope::FUNC_PROTOTYPE);
    for (auto param : dc->getParmVarDeclList()) {
        if (nullptr == param) {
            semaError("parameter declaration expected!");
            return;
        }
        if (nullptr == sys_->insertNormal(param->getName(), param->getType(), param)) {
            semaError("parameter declaration expected!");
            return;
        }
    }
    // 解析函数体
    dc->setBody(parseCompoundStmt());
}


/*(6.9) translation-unit:
 external-declaration
 translation-unit external-declaration
*/
void Parser::parseTranslationUnit()
{
    ScopeManager scm(this, Scope::FILE);
    sys_->initBuiltType();
    DeclGroup res;
    while (!match(TokenKind::EOF_)) 
    {
        if (match(TokenKind::Semicolon_)) 
        {
            continue;
        }
        auto path = parseDeclaration();
        res.insert(res.begin(), path.begin(), path.end());
    }
    unit_ = new TranslationUnitDecl(res, sys_->getCurScope());
}
