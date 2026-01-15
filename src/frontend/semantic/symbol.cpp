#include "symbol.h"
#include "decl.h"

std::string Symbol::getTag() 
{
    return getTag(m_nameSpace, m_name);
}

std::string Symbol::getTag(NameSpace st, const std::string& key)
{
    std::string res(key);
    switch (st)
    {
    case LABEL:
        res.append("@LABEL");
        break;
    case RECORD:
        res.append("@RECORD");
        break;
    case MEMBER:
        res.append("@RECORD_MEMBER");
        break;
    case NORMAL:
    default:
        res.append("@NORMAL");
        break;
    }
    return res;
}

Scope::Scope(ScopeType st, std::shared_ptr<Scope> parent)
: m_scopeType(st), m_parent(parent)
{
    m_level = parent ? (parent->getLevel() + 1) : 1;
}

std::shared_ptr<Symbol> Scope::lookup(Symbol::NameSpace st, const std::string& key)
{
    auto name = Symbol::getTag(st, key);
    if (m_sysbolTable.count(name)) {
        return m_sysbolTable[name];
    }
    if (m_parent) {
        return m_parent->lookup(st, key);
    }
    return nullptr;
}

bool Scope::insert(std::shared_ptr<Symbol> sys)
{
    if (!sys) {
        return false;
    }
    auto name = sys->getTag();
    if (!m_sysbolTable.count(name)) {
        m_sysbolTable[name] = sys;
        return true;
    }
    return false;
}

bool SymbolTableContext::insertLabel(std::shared_ptr<Symbol> sys)
{
    if (!sys) {
        return false;
    }
    sys->setNameSpace(Symbol::LABEL);
    return m_curScope->insert(sys);
}

bool SymbolTableContext::insertRecord(std::shared_ptr<Symbol> sys)
{
    if (!sys) return false;
    sys->setNameSpace(Symbol::RECORD);
    return m_curScope->insert(sys);
}

bool SymbolTableContext::insertMember(std::shared_ptr<Symbol> sys)
{
    if (!sys) return false;
    sys->setNameSpace(Symbol::MEMBER);
    return m_curScope->insert(sys);
}

bool SymbolTableContext::insertNormal(std::shared_ptr<Symbol> sys)
{
    if (!sys) return false;
    sys->setNameSpace(Symbol::NORMAL);
    sys->setScope(m_curScope);
    return m_curScope->insert(sys);
}

std::shared_ptr<Symbol> SymbolTableContext::LookupLabel(const std::string& name)
{
    return m_curScope->lookup(Symbol::LABEL, name);
}
std::shared_ptr<Symbol> SymbolTableContext::LookupRecord(const std::string& name)
{
    return m_curScope->lookup(Symbol::RECORD, name);
}
std::shared_ptr<Symbol> SymbolTableContext::LookupMember(const std::string& name)
{
    return m_curScope->lookup(Symbol::MEMBER, name);
}
std::shared_ptr<Symbol> SymbolTableContext::LookupNormal(const std::string& name)
{
    return m_curScope->lookup(Symbol::NORMAL, name);
}

void SymbolTableContext::enterFileScope()
{
    m_curScope = std::make_shared<Scope>(Scope::FILE, m_curScope);
}
void SymbolTableContext::enterFuncScope()
{
    m_curScope = std::make_shared<Scope>(Scope::FUNC, m_curScope);
}
void SymbolTableContext::enterBlockScope()
{
    m_curScope = std::make_shared<Scope>(Scope::BLOCK, m_curScope);
}
void SymbolTableContext::enterFuncPrototypeScope()
{
    m_curScope = std::make_shared<Scope>(Scope::FUNC_PROTOTYPE, m_curScope);
}
void SymbolTableContext::exitScope()
{
    if (!m_curScope) {
        throw std::string("curScope_ is nullptr");
    }
    m_curScope = m_curScope->getParent();
}

/*
(6.7.7) type-name:
 specifier-qualifier-list abstract-declaratoropt
  (6.7.2.1) specifier-qualifier-list:
 type-specifier specifier-qualifier-listopt
 type-qualifier specifier-qualifier-listopt
*/
bool SymbolTableContext::isTypeName(Token* tk)
{
    if (!tk) {
        return false;
    }
    return (isTypeSpecifier(tk) || isTypeQualifier(tk));
}

bool SymbolTableContext::isTypeSpecifier(Token* tk)
{
    if (!tk) {
        return false;
    }
    // 判断是否是类型说明
    switch (tk->kind_)
    {
    case TokenKind::Void:
    case TokenKind::Char:
    case TokenKind::Short:
    case TokenKind::Int:
    case TokenKind::Long:
    case TokenKind::Float:
    case TokenKind::Double:
    case TokenKind::Signed:
    case TokenKind::Unsigned:
    case TokenKind::T_Bool:
    case TokenKind::T_Complex:
    /* atomic-type-specifier*/
    /*struct-or-union-specifier*/
    case TokenKind::Struct:
    case TokenKind::Union:
    /*enum-specifier*/
    case TokenKind::Enum:
        return true;
    default:
        break;
    }
    /*typedef-name*/
    auto sym = LookupNormal(tk->value_);
    if (sym && sym->isTypeName()) {
        return true;
    }
    return false;
}

bool SymbolTableContext::isTypeQualifier(Token* tk)
{
    if (!tk) return false;
    // 判断是否是类型限定符
    switch (tk->kind_)
    {
    case TokenKind::Const:
    case TokenKind::Volatile:
    case TokenKind::Restrict:
    case TokenKind::T_Atomic:
        return true;
    default:
        break;
    }
    return false;
}   

void SymbolTableContext::initBuiltType()
{
    auto insert = [&](const std::string& name, std::shared_ptr<Type> ty){
        auto sys = std::make_shared<Symbol>();
        sys->setName(name);
        sys->setType(QualType(ty));
        insertNormal(sys);
    };
    // void
    insert("void", std::make_shared<VoidType>());
    // bool
    insert("_Bool", std::make_shared<BoolType>());
    // char型
    insert("signed_char", std::make_shared<IntegerType>(IntegerType::SIGNED, IntegerType::BYTE, IntegerType::CHAR));
    insert("unsignd_char", std::make_shared<IntegerType>(IntegerType::UNSIGNED, IntegerType::BYTE, IntegerType::CHAR));
    // 整型
    insert("signed_short_int", std::make_shared<IntegerType>(IntegerType::SIGNED, IntegerType::SHORT, IntegerType::INT));
    insert("signed_normal_int", std::make_shared<IntegerType>(IntegerType::SIGNED, IntegerType::NORMAL, IntegerType::INT));
    insert("signed_long_int", std::make_shared<IntegerType>(IntegerType::SIGNED, IntegerType::LONG, IntegerType::INT));
    insert("signed_long_long_int", std::make_shared<IntegerType>(IntegerType::SIGNED, IntegerType::LONG2, IntegerType::INT));
    insert("unsignd_short_int", std::make_shared<IntegerType>(IntegerType::UNSIGNED, IntegerType::SHORT, IntegerType::INT));
    insert("unsignd_normal_int", std::make_shared<IntegerType>(IntegerType::UNSIGNED, IntegerType::NORMAL, IntegerType::INT));
    insert("unsignd_long_int", std::make_shared<IntegerType>(IntegerType::UNSIGNED, IntegerType::LONG, IntegerType::INT));
    insert("unsigned_long_long_int", std::make_shared<IntegerType>(IntegerType::UNSIGNED, IntegerType::LONG, IntegerType::INT));
    // 浮点型
    insert("float", std::make_shared<RealFloatingType>(RealFloatingType::FLOAT));
    insert("double", std::make_shared<RealFloatingType>(RealFloatingType::DOUBLE));
    insert("long_double", std::make_shared<RealFloatingType>(RealFloatingType::LONG_DOUBLE));
    // 复数
    insert("float_complex", std::make_shared<ComplexType>(ComplexType::FLOAT));
    insert("double_complex", std::make_shared<ComplexType>(ComplexType::DOUBLE));
    insert("long_double_complex", std::make_shared<ComplexType>(ComplexType::LONG_DOUBLE));
}

std::shared_ptr<Type> SymbolTableContext::getBuiltTypeByTS(int tq)
{
    std::string key;
    if (tq & VOID) {
        key = "void";
    }
    else if (tq & _BOOL) {
        key = "_Bool";
    }
    else if (tq & CHAR) {
        key = (tq & UNSIGNED) ? "unsigned_" : "signed_";
        key.append("char");
    }
    else if (tq & FLOAT) {
        key = (tq & _COMPLEX) ? "float_complex" : "float";
    }
    else if (tq & DOUBLE) {
        key = (tq & LONG) ? "long_double" : "double";
        if (tq & _COMPLEX) {
            key.append("_complex");
        }
    }
    else {
        key = (tq & UNSIGNED) ? "unsigned_" : "signed_";
        if (tq & SHORT) {
            key.append("short_");
        } 
        else if (tq & LONG) {
            key.append("long_");
        }
        else if (tq & LONGLONG) {
            key.append("long_long_");
        }
        else {
            key.append("normal_");
        }
        key.append("int");
    }
    auto sys = LookupNormal(key);
    if (sys) {
        return sys->getType().getPtr();
    }
    return nullptr;
}
