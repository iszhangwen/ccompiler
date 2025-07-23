#include "parse.h"
#include <sema.h>
#include <sstream>

void ParseTypeSpec::operator()(TSState& curState, int* ts, TokenKind cond)
{
    switch (cond)
    {
    case TokenKind::Void:
        if (curState == TSState::START) {
            curState = TSState::FOUND_TYPE;
            *ts |= TypeSpecifier::VOID;
        } else {
            curState = TSState::ERROR;
        }
        break;

    case TokenKind::Char:
        if (curState == TSState::START 
            || curState == TSState::FOUND_SIGNED_UNSIGNED) {
            curState = TSState::FOUND_TYPE;
            *ts |= TypeSpecifier::CHAR;
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
            *ts |= TypeSpecifier::INT;
        } else {
            curState = TSState::ERROR;
        }
        break;

    case TokenKind::Float:
        if (curState == TSState::START) {
            curState = TSState::FOUND_TYPE;
            *ts |= TypeSpecifier::FLOAT;
        } else {
            curState = TSState::ERROR;
        }
        break;

    case TokenKind::Double:
        if (curState == TSState::START) {
            *ts |= TypeSpecifier::DOUBLE;
        }
        else if (curState == TSState::FOUND_LONG) {
            *ts &= ~(TypeSpecifier::LONG);
            *ts |= TypeSpecifier::LONGDOUBLE;
        }
        else {
            curState = TSState::ERROR;
        }
        break;

    case TokenKind::Signed:
        if (curState == TSState::START) {
            curState = TSState::FOUND_SIGNED_UNSIGNED;
            *ts |= TypeSpecifier::SIGNED;
        } else {
            curState = TSState::ERROR;
        }
        break;

    case TokenKind::Unsigned:
        if (curState == TSState::START) {
            curState = TSState::FOUND_SIGNED_UNSIGNED;
            *ts |= TypeSpecifier::UNSIGNED;
        } else {
            curState = TSState::ERROR;
        }
        break;

    case TokenKind::Short:
        if (curState == TSState::START) {
            curState = TSState::FOUND_SHORT;
            *ts |= TypeSpecifier::SHORT;
        } else {
            curState = TSState::ERROR;
        }
        break;

    case TokenKind::Long:
        if (curState == TSState::START
            || curState == FOUND_SIGNED_UNSIGNED) {
            curState = TSState::FOUND_LONG;
            *ts |= TypeSpecifier::LONG;
        } else if (curState == TSState::FOUND_LONG) {
            curState = FOUND_LONG2;
            *ts &= ~(TypeSpecifier::LONG);
            *ts |= TypeSpecifier::LONGLONG;
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
    switch (seq_->next()->kind_)
    {
    case TokenKind::Identifier:
        node = sema_->onActDeclRefExpr(seq_->cur());
        break;

    case TokenKind::Numeric_Constant_:
    case TokenKind::Float_Constant:
    case TokenKind::Character_Constant_:
    case TokenKind::String_Constant_:
        node = sema_->onActConstant(seq_->cur());
        break;
    
    case TokenKind::LParent_:
        node = parseExpr();
        seq_->expect(TokenKind::RParent_);
        node = sema_->onActParenExpr(node);
        break;

    default:
        error(seq_->cur(), "unexpect PrimaryExpr!");
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
    seq_->expect(TokenKind::T_Generic);
    seq_->expect(TokenKind::LParent_);
    parseAssignExpr();
    while (seq_->match(TokenKind::Comma_)) {
        parseGenericAssociation();
    }
    seq_->expect(TokenKind::RParent_);
    return nullptr;
}

/*(6.5.1.1) generic-association:
 type-name : assignment-expression
 default : assignment-expression
*/
Expr* Parser::parseGenericAssociation()
{
    if (seq_->match(TokenKind::Default)) {

    }
    else {
        parseTypeName();
    }
    seq_->match(TokenKind::Colon_);
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
        if (seq_->match(TokenKind::LSquare_Brackets_)) {
            auto lex = parseExpr();
            seq_->expect(TokenKind::RSquare_Brackets_);
            node = sema_->onActArraySubscriptExpr(node, lex);
        }
        else if (seq_->match(TokenKind::LParent_)) {
            auto lex = parseArgListExpr();
            seq_->expect(TokenKind::RParent_);
            node = sema_->onActCallExpr(node, lex);
        }
        else if (seq_->match(TokenKind::Dot_)) {
            seq_->expect(TokenKind::Identifier);
            auto lex = sema_->onActDeclRefExpr(seq_->cur());
            node = sema_->onActMemberExpr(node, lex, false);
        }
        else if (seq_->match(TokenKind::Arrow_)) {
            seq_->expect(TokenKind::Identifier);
            auto lex = sema_->onActDeclRefExpr(seq_->cur());
            node = sema_->onActMemberExpr(node, lex, true);
        }
        else if (seq_->match(TokenKind::Increment_)) {
            node = sema_->onActUnaryOpExpr(node, UnaryOpExpr::Post_Increment_);
        }
        else if (seq_->match(TokenKind::Decrement_)) {
            node = sema_->onActUnaryOpExpr(node, UnaryOpExpr::Post_Decrement_);
        }
        else {
            break;
        }
    }
    return node;
}

Expr* Parser::parseArgListExpr()
{
    Expr* node = parseAssignExpr();
    while (seq_->match(TokenKind::Comma_)) {
        auto rex = parseAssignExpr();
        node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Comma);
    }
    return node;
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
    switch (seq_->next()->kind_)
    {
    case TokenKind::Increment_:
        rex = parseUnaryExpr();
        node = sema_->onActUnaryOpExpr(rex, UnaryOpExpr::Pre_Increment_);
        break;

    case TokenKind::Decrement_:
        rex = parseUnaryExpr();
        node = sema_->onActUnaryOpExpr(rex, UnaryOpExpr::Pre_Decrement_);
        break;

    case TokenKind::BitWise_AND_:
        rex = parseCastExpr();
        node = sema_->onActUnaryOpExpr(rex, UnaryOpExpr::BitWise_AND_);
        break;

    case TokenKind::Multiplication_:
        rex = parseCastExpr();
        node = sema_->onActUnaryOpExpr(rex, UnaryOpExpr::Multiplication_);
        break;

    case TokenKind::Addition_:
        rex = parseCastExpr();
        node = sema_->onActUnaryOpExpr(rex, UnaryOpExpr::Addition_);
        break;

    case TokenKind::Subtraction_:
        rex = parseCastExpr();
        node = sema_->onActUnaryOpExpr(rex, UnaryOpExpr::Subtraction_);
        break;

    case TokenKind::BitWise_NOT_:
        rex = parseCastExpr();
        node = sema_->onActUnaryOpExpr(rex, UnaryOpExpr::BitWise_NOT_);
        break;

    case TokenKind::Logical_NOT_:
        rex = parseCastExpr();
        node = sema_->onActUnaryOpExpr(rex, UnaryOpExpr::Logical_NOT_);
        break;
    
    case TokenKind::Sizeof:
        if (seq_->match(TokenKind::RParent_)) {
            if (sys_->isTypeName(seq_->peek())) {
                parseTypeName();
                rex = sema_->onActParenExpr(nullptr);
            }
            else {
                rex = parseUnaryExpr();
            }
            seq_->expect(TokenKind::RParent_);
        }
        else {
            rex = parseUnaryExpr();
        }
        node = sema_->onActUnaryOpExpr(rex, UnaryOpExpr::Logical_NOT_);
        break;
    
    case TokenKind::Alignof:
        seq_->expect(TokenKind::LParent_);
        parseTypeName();
        seq_->expect(TokenKind::RParent_);
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
        if (seq_->match(TokenKind::Multiplication_)) {
            Expr* rex = parseCastExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Multiplication_);
        } 
        else if (seq_->match(TokenKind::Division_)) {
            Expr* rex = parseCastExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Division_);
        } 
        else if (seq_->match(TokenKind::Modulus_)) {
            Expr* rex = parseCastExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Modulus_);
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
        if (seq_->match(TokenKind::Addition_)) {
            Expr* rex = parseMultiExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Addition_);
        } 
        else if (seq_->match(TokenKind::Subtraction_)) {
            Expr* rex = parseMultiExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Subtraction_);
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
        if (seq_->match(TokenKind::LShift_)) {
            Expr* rex = parseAddExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::LShift_);
        } 
        else if (seq_->match(TokenKind::RShift_)) {
            Expr* rex = parseAddExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::RShift_);
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
        if (seq_->match(TokenKind::Less_)) {
            Expr* rex = parseShiftExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Less_);
        } 
        else if (seq_->match(TokenKind::Less_Equal_)) {
            Expr* rex = parseShiftExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Less_Equal_);
        } 
        else if (seq_->match(TokenKind::Greater_)) {
            Expr* rex = parseShiftExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Greater_);
        } 
        else if (seq_->match(TokenKind::Greater_Equal_)) {
            Expr* rex = parseShiftExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Greater_Equal_);
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
        if (seq_->match(TokenKind::Equality_)) {
            Expr* rex = parseRelationalExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Equality_);
        } 
        else if (seq_->match(TokenKind::Inequality_)) {
            Expr* rex = parseRelationalExpr();
            node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Inequality_);
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
    while (seq_->match(TokenKind::BitWise_AND_)) {
        Expr* rex = parseEqualExpr();
        node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::BitWise_AND_);
    }
    return node;
}

Expr* Parser::parseBitXORExpr()
{
    Expr* node = parseBitANDExpr();
    while (seq_->match(TokenKind::BitWise_XOR_)) {
        Expr* rex = parseBitANDExpr();
        node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::BitWise_XOR_);
    }
    return node;
}

Expr* Parser::parseBitORExpr()
{
    Expr* node = parseBitXORExpr();
    while (seq_->match(TokenKind::BitWise_OR_)) {
        Expr* rex = parseBitXORExpr();
        node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::BitWise_OR_);
    }
    return node;
}

Expr* Parser::parseLogicalANDExpr()
{
    Expr* node = parseBitORExpr();
    while (seq_->match(TokenKind::Logical_AND_)) {
        Expr* rex = parseBitORExpr();
        node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Logical_AND_);
    }
    return node;
}

Expr* Parser::parseLogicalORExpr()
{
    Expr* node = parseLogicalANDExpr();
    while (seq_->match(TokenKind::Logical_OR_)) {
        Expr* rex = parseLogicalANDExpr();
        node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Logical_OR_);
    }
    return node;
}

Expr* Parser::parseConditionalExpr()
{
    Expr* node = parseLogicalORExpr();
    if (seq_->match(TokenKind::Conditional_)) {
        Expr* th = parseExpr();
        seq_->expect(TokenKind::Colon_);
        Expr* el = parseConditionalExpr();
        node = sema_->onActConditionalExpr(node, th, el);
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
    switch (seq_->next()->kind_)
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
        error("expect assignment-operator, but not!");
        break;
    }
    return sema_->onActBinaryOpExpr(node, rex, opc);
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
    while (seq_->match(TokenKind::Comma_)) {
        auto rnode = parseAssignExpr();
        node = sema_->onActBinaryOpExpr(node, rnode, BinaryOpExpr::Comma);
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
    if (!seq_->match(TokenKind::LParent_)) {
        error(seq_->cur(), "expect Lparen, but not Lparen!");
        return nullptr;
    }
    
    Expr* node = nullptr;
    if (sys_->isTypeName(seq_->peek())) {
        //QualType qt = parseTypeName();
        seq_->expect(TokenKind::RParent_);
        if (seq_->match(TokenKind::LCurly_Brackets_)) {
            //node = parseInitlizerList();
            while (seq_->match(TokenKind::Comma_)) {
                //auto rex = parseInitlizerList();
                //node = sema_->onActBinaryOpExpr(node, rex, BinaryOpExpr::Comma);
            }
            seq_->expect(TokenKind::RCurly_Brackets_);
            node = sema_->onActCompoundLiteralExpr();
        }
        else {
            auto lex = parseCastExpr();
            //node = sema_->onActCastExpr(qt, lex);
        }
    }
    else {
        auto lex = parseExpr();
        seq_->expect(TokenKind::RParent_);
        node = sema_->onActParenExpr(lex);
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
    // typedef struct union enum 解析完成
    if (seq_->match(TokenKind::Semicolon_)) {
        res.push_back(qt->getDecl());
        return res;
    }
    // 解析第一个声明符
    res.push_back(parseInitDeclarator(qt, sc, fs));
    if (qt->isFunctionType() && seq_->test(TokenKind::LCurly_Brackets_)) {
        res.push_back(parseFunctionDefinitionBody(res[0]));
        return res;
    }

    while (seq_->match(TokenKind::Comma_)) {
            res.push_back(parseInitDeclarator(qt, sc, fs));
    }
    seq_->expect(TokenKind::Semicolon_);
    return res;
}

QualType Parser::parseDeclarationSpec(int* sc, int* fs)
{
    int tq = 0; // 类型限定符
    int ts = 0; // 类型说明符
    Type* ty = nullptr; // 类型：内建类型,自定义类型
    ParseTypeSpec::TSState tss = ParseTypeSpec::START; // 类型说明符解析：状态机状态
    while (true)
    {
        TokenKind tkind = seq_->next()->kind_;
        switch (tkind)
        {
        // (6.7.1) storage class specifier
        case TokenKind::Typedef:
        case TokenKind::Extern:
        case TokenKind::Static:
        case TokenKind::Auto:
        case TokenKind::Register:
            parseStorageClassSpec(sc, tkind);
            break;

        // (6.7.2) type specifiers
        // 使用状态机解析TypeSpecifier
        case TokenKind::T_Bool:
        case TokenKind::T_Complex:
            break;
        case TokenKind::Void:
        case TokenKind::Char:
        case TokenKind::Int:
        case TokenKind::Float:
        case TokenKind::Double:
        case TokenKind::Signed:
        case TokenKind::Unsigned:
        case TokenKind::Short:
        case TokenKind::Long:
            ParseTypeSpec()(tss, &ts, seq_->cur()->kind_);
            if (tss == ParseTypeSpec::FOUND_TYPE) {
                ty = sema_->onActBuiltinType(ts);
            } else if (tss == ParseTypeSpec::ERROR) {
                error("error type specifier!");
            }
            break;

        // (6.7.2) type-specifier->struct-or-union-specifier
        case TokenKind::Struct:
        case TokenKind::Union:
            if (tss == ParseTypeSpec::START) {
                ty = parseStructOrUnionSpec(seq_->cur()->kind_ == TokenKind::Struct);
                tss = ParseTypeSpec::FOUND_UDT_TYPE;
            }
            else {
                error("the type-specifier has discovered, there is a conflict!");
                tss = ParseTypeSpec::ERROR;
            }
            break;

        // (6.7.2) type-specifier->enum-specifier
        case TokenKind::Enum:
            if (tss == ParseTypeSpec::START) {
                ty = parseEnumSpec();
                tss = ParseTypeSpec::FOUND_UDT_TYPE;
            }
            else {
                error("the type-specifier has discovered, there is a conflict!");
                tss = ParseTypeSpec::ERROR;
            }
            break;

        //(6.7.3) type-qualifier:
        case TokenKind::Const:    tq |= TypeQualifier::CONST; break;
        case TokenKind::Volatile: tq |= TypeQualifier::VOLATILE; break;
        case TokenKind::Restrict: tq |= TypeQualifier::RESTRICT; break;

        // (6.7.4) function-specifier
        case TokenKind::Inline:
            parseFunctionSpec(fs, seq_->cur()->kind_);
            break;

        // (6.7.7) typedef-name 判断当前是否已有其他方式
        case TokenKind::Identifier:
            if (tss == ParseTypeSpec::START) {
                //Decl* dc = sys_->lookup(Symbol::NORMAL, seq_->cur()->value_);
                //qt = dc->getType();
                tss = ParseTypeSpec::FOUND_UDT_TYPE;
                break;
            } 
        default:
            if (!ty) {
                error("incomplete type specifier!");
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
Decl* Parser::parseInitDeclarator(QualType qt, int sc, int fs)
{
    Decl* dc = parseDeclarator(qt, sc, fs);
    if (seq_->match(TokenKind::Assign_)) {
        parseInitializer();
    }
    return dc;
}

void Parser::parseStorageClassSpec(int* sc, TokenKind tk)
{
    if (sc == nullptr) {
        error("expect not storageclass, but has!");
        return;
    } 
    else if (sc != 0) {
        error("duplication storageclass!");
        return;
    }
    switch (tk)
    {
    case TokenKind::Typedef:
        *sc |= StorageClass::TYPEDEF;
        break;
    case TokenKind::Extern:
        *sc |= StorageClass::EXTERN;
        break;
    case TokenKind::Static:
        *sc |= StorageClass::STATIC;
        break;
    case TokenKind::Auto:
        *sc |= StorageClass::AUTO;
        break;
    case TokenKind::Register:
        *sc |= StorageClass::REGISTER;
        break;
    
    default:
        error("expect storageclass, but not!");
        break;
    }
}

/* (6.7.2.1) struct-or-union-specifier:
 struct-or-union identifieropt { struct-declaration-list }
 struct-or-union identifier
*/
Type* Parser::parseStructOrUnionSpec(bool isStruct)
{
    //符号解析
    std::string key;
    if (seq_->match(TokenKind::Identifier)) {
        key = seq_->cur()->value_;
    }

    Symbol* sym = sys_->lookup(Symbol::RECORD, key);
    // UDT定义
    if (seq_->match(TokenKind::LCurly_Brackets_)) {
        // 符号表没查找到:第一次定义
        if (!sym) {
            Type* ty = sema_->onActRecordType(isStruct, nullptr);
            if (key.empty()) { // 匿名对象不插入符号表
                sys_->insert(Symbol::RECORD, key, t);
            }
            return parseStructDeclarationList(ty);
        }
        // 符号表查找到了但是类型定义不完整：存在前向声明
        else if (sym->getType()->isIncompleteType()) {
            return parseStructDeclarationList(sym->getType());
        }
        // 符号表查找到了并且类型定义完整：重复定义
        else {
            error("redefined struct or union!");
            return nullptr;
        }
    }
    // UDT前向声明 struct test;
    // struct test *p;
    // 非定义情况下使用UDT必须要有声明符
    if (key.empty()) {
        error("struct or union need identifier, but not!");
        return nullptr;
    }
    if (sym) {
        return sym->getType();
    }
    Type* ty = sema_->onActRecordType(isStruct, nullptr);
    sys_->insert(Symbol::RECORD, key, ty);
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
    RecordType* ty = dynamic_cast<RecordType*>(sym->getType());
    RecordDecl* dc = sema_->onActRecordDecl(sym, true, ty->isStruct());
    do {
        QualType qt = parseSpecQualList();
        DeclGroup path = parseStructDeclaratorList(qt);
        dc->addField(path);
        seq_->match(TokenKind::Semantics);
    }while (seq_->test(TokenKind::RCurly_Brackets_));
    ty->setDecl(ty);
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
DeclGroup Parser::parseStructDeclaratorList(QualType qt)
{
    DeclGroup res;
    do {
        if (seq_->match(TokenKind::Colon_)) {
            parseConstansExpr();
        }
        else {
            Decl* dc = parseDeclarator(qt, 0, 0);
            if (seq_->match(TokenKind::Colon_)) {
                parseConstansExpr();
            }
        }

    }while (seq_->match(TokenKind::Semicolon_));
    return res;
}

/* (6.7.2.2) enum-specifier:
 enum identifieropt { enumerator-list }
 enum identifieropt { enumerator-list ,}
 enum identifier
  (6.7.2.2) enumerator-list:
 enumerator
 enumerator-list , enumerator
*/
Type* Parser::parseEnumSpec()
{
    // 符号解析
    std::string key;
    if (seq_->match(TokenKind::Identifier)) {
        key = seq_->cur()->value_;
    }

    // 枚举定义解析
    if (seq_->match(TokenKind::LCurly_Brackets_)) {
        // 打开新作用域
        ScopeManager scm(this, Scope::FILE);
        while (true) {
            parseEnumerator();
            // 匹配到逗号
            if (seq_->match(TokenKind::Comma_)) {
                if (seq_->match(TokenKind::RCurly_Brackets_)) {
                    break;
                }
                continue;
            }
            // 未匹配到逗号，则必定结束
            else {
                seq_->expect(TokenKind::RCurly_Brackets_);
                break;
            }
        }
        return nullptr; 
    }
    // 枚举声明或使用
    // 必须要定义符号
    if (key.empty()) {
        error("expect enum identifier, but not!");
        return nullptr;
    }
    sema_
    return nullptr;
}

/*(6.7.2.2) enumerator:
 enumeration-constant
 enumeration-constant = constant-expression
  (6.4.4.3) enumeration-constant:
 identifier
*/
void Parser::parseEnumerator()
{
    seq_->expect(TokenKind::Identifier);
    if (seq_->match(TokenKind::Assign_))
    {
        parseConstansExpr();
    }
    return;
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
void Parser::parseFunctionSpec(int* fs, TokenKind tk)
{
    if (fs == nullptr) {
        error("expect not function specifier, but has!");
        return;
    } 
    else if (fs != 0) {
        error("duplication function specifier!");
        return;
    }
    
    switch (tk)
    {
    case TokenKind::Inline:
        *fs |= FuncSpecifier::INLINE;
        break;
    
    default:
        error("expect function specifier, but not!");
        break;
    }
}

/*declarator:
 pointeropt direct-declarator
*/
Decl* Parser::parseDeclarator(QualType qt, int sc, int fs)
{
    if (seq_->test(TokenKind::Multiplication_)) {
        qt = parsePointer(qt);
    }

    parseDirectDeclarator();
    return nullptr;
}

void Parser::parseDirectDeclarator()
{
 
}

/*(6.7.5) pointer:
 * type-qualifier-listopt
 * type-qualifier-listopt pointer
*/
QualType Parser::parsePointer(QualType qt)
{ 
    while (seq_->match(TokenKind::Multiplication_)) {
        Type* ty = sema_->onActPointerType(qt);
        int tq = parseTypeQualList();
        qt = QualType(ty, tq);
    }
    return qt;
}

void Parser::parseParameterTypeList()
{
    parseParameterList();
    if (seq_->match(TokenKind::Comma_)) {
        seq_->expect(TokenKind::Ellipsis_);
    }
}

void Parser::parseParameterList()
{
    parseParameterDeclaration();
    while (seq_->match(TokenKind::Comma_)) {
        parseParameterDeclaration();
    }
    return;
}

/* (6.7.5) parameter-declaration:
 declaration-specifiers declarator
 declaration-specifiers abstract-declaratoropt
*/
void Parser::parseParameterDeclaration()
{

}

/* (6.7.5) identifier-list:
 identifier
 identifier-list , identifier
*/
void Parser::parseIdentifierList()
{
    seq_->expect(TokenKind::Identifier);
    while (seq_->match(TokenKind::Identifier)) {
        seq_->expect(TokenKind::Identifier);
    }
}

/*(6.7.6) type-name:
 specifier-qualifier-list abstract-declaratoropt
*/
void Parser::parseTypeName()
{
    parseSpecQualList();
    parseAbstractDeclarator();
}

/* (6.7.6) abstract-declarator:
 pointer
 pointeropt direct-abstract-declarator
*/
void Parser::parseAbstractDeclarator()
{
    parsePointer();
    parseDirectAbstractDeclarator();
}

void Parser::parseDirectAbstractDeclarator()
{

}

/* (6.7.7) typedef-name:
 identifier
*/
void Parser::parseTypedefName()
{
    seq_->expect(TokenKind::Identifier);
}

/*(6.7.8) initializer:
 assignment-expression
 { initializer-list }
 { initializer-list , }
*/
void Parser::parseInitializer()
{
    if (seq_->match(TokenKind::LCurly_Brackets_)) {
        parseInitializerList();
        seq_->match(TokenKind::Comma_);
        seq_->expect(TokenKind::RCurly_Brackets_);
    }
    else {
        parseAssignExpr();
    }
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
    } while (seq_->match(TokenKind::Comma_));
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
    seq_->expect(TokenKind::Assign_);
}

void Parser::parseDesignator()
{
    if (seq_->match(TokenKind::LSquare_Brackets_)) {
        parseConstansExpr();
        seq_->expect(TokenKind::LSquare_Brackets_);
    }
    else if (seq_->match(TokenKind::Dot_)) {
        seq_->expect(TokenKind::Identifier);
    }
    else {
        error("expect designator, but not!");
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
    switch (seq_->next()->kind_)
    {
    case TokenKind::Identifier:
    {
        NamedDecl* key = nullptr;
        seq_->expect(TokenKind::Colon_);
        Stmt* body = parseStmt();
        node = sema_->onActLabelStmt();
        break;
    }
    
    case TokenKind::Case:
    {
        Expr* cond = parseConstansExpr();
        seq_->expect(TokenKind::Colon_);
        Stmt* body = parseStmt();
        node = sema_->onActCaseStmt();
        break;
    }

    case TokenKind::Default:
    {
        seq_->expect(TokenKind::Colon_);
        Stmt* body = parseStmt();
        node = sema_->onActDefaultStmt();
        break;
    }

    default:
        error("expect label, but not!");
        return nullptr;
    }
    return node;
}

Stmt* Parser::parseCompoundStmt()
{
    seq_->expect(TokenKind::LCurly_Brackets_);
    Stmt* node = sema_->onActCompoundStmt();
    while (seq_->match(TokenKind::RCurly_Brackets_)) 
    {
        // 
    }
    return node;    
}

Stmt* Parser::parseExprStmt()
{
    Expr* ex = parseExpr();
    seq_->expect(TokenKind::Semicolon_);
    return sema_->onActExprStmt();
}

Stmt* Parser::parseSelectionStmt()
{
    Stmt* node = nullptr;
    switch (seq_->next()->kind_)
    {
    case TokenKind::If:
    {   
        seq_->expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        seq_->expect(TokenKind::RParent_);
        Stmt* th = parseStmt();
        Stmt* el = (seq_->match(TokenKind::Else)) ? parseStmt() : nullptr;
        node = sema_->onActIfStmt();
        break;
    }

    case TokenKind::Switch:
    {
        seq_->expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        seq_->expect(TokenKind::RParent_);
        Stmt* body = parseStmt();
        node = sema_->onActSwitchStmt();
        break;
    }
    
    default:
        error("expect selection, but not!");
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
        seq_->expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        seq_->expect(TokenKind::RParent_);
        Stmt* body = parseStmt();
        node = sema_->onActWhileStmt();
        break;
    }

    case TokenKind::Do:
    {
        Stmt* body = parseStmt();
        seq_->expect(TokenKind::While);
        seq_->expect(TokenKind::LParent_);
        Expr* cond = parseExpr();
        seq_->expect(TokenKind::RParent_);
        seq_->expect(TokenKind::Semicolon_);
    }

    case TokenKind::For:
        /* code */
        break;
    
    default:
        error("expect iteration, but not!");
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
        seq_->expect(TokenKind::Identifier);
        seq_->expect(TokenKind::Semicolon_);
        node = sema_->onActGotoStmt();
        break;
    
    case TokenKind::Continue:
        seq_->expect(TokenKind::Semicolon_);
        node = sema_->onActContinueStmt();
        break;

    case TokenKind::Break:
        seq_->expect(TokenKind::Semicolon_);
        node = sema_->onActBreakStmt();
        break;

    case TokenKind::Return:
    {
        auto rex = parseExprStmt();
        node = sema_->onActReturnStmt();
        break;
    }
    
    default:
        error("expect jump, but not!");
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
{
    FunctionDecl* fd= dynamic_cast<FunctionDecl*>(dc);
    Stmt* body = parseCompoundStmt();
    fd->setBody(body);
    return fd;
}


/*(6.9) translation-unit:
 external-declaration
 translation-unit external-declaration
*/
void Parser::parseTranslationUnit()
{
    ScopeManager scm(this, Scope::FILE);
    DeclGroup res;
    while (!seq_->match(TokenKind::EOF_)) 
    {
        if (seq_->match(TokenKind::Semicolon_)) 
        {
            continue;
        }
        auto rpath = parseDeclaration();
        res.insert(res.begin(), rpath.begin(), rpath.end());
    }
    unit_ = sema_->onActTranslationUnitDecl(res);
}
