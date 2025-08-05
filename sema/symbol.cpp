#include "symbol.h"
#include <ast/decl.h>

Symbol* Symbol::NewObj(SymbolType st, Scope* s, const std::string& k, QualType t, NamedDecl* dc, bool isType)
{
    return new Symbol(st, s, k, t, dc, isType);
}

std::string Symbol::getTag() 
{
    return getTag(st_, key_);
}

std::string Symbol::getTag(SymbolType st, const std::string& key)
{
    std::string tail;
    switch (st)
    {
    case LABEL:
        tail.append("@LABEL");
        break;
    case RECORD:
        tail.append("@RECORD");
        break;
    case MEMBER:
        tail.append("@RECORD_MEMBER");
        break;
    case NORMAL:
    default:
        tail.append("@NORMAL");
        break;
    }
    return key + tail;
}

Scope::Scope(ScopeType st, Scope* parent)
: sct_(st), parent_(parent)
{
    level_ = parent ? (parent->getLevel() + 1) : 1;
}

Symbol* Scope::lookup(Symbol::SymbolType st, const std::string& key)
{
    std::string tmp = Symbol::getTag(st, key);
    if (table_.count(tmp)) {
        return table_[tmp];
    }
    if (!parent_) {
        return parent_->lookup(st, key);
    }
    return nullptr;
}

bool Scope::insert(Symbol* sy)
{
    if (!sy) {
        return false;
    }
    std::string tmp = sy->getTag();
    if (!table_.count(tmp)) {
        table_[tmp] = sy;
        return true;
    }
    return false;
}

Symbol* SymbolTableContext::insert(Symbol::SymbolType st, const std::string& key, QualType ty, NamedDecl* dc,  bool isType)
{
    if (!curScope_) {
        return nullptr;
    }
    Symbol* sym = Symbol::NewObj(st, curScope_, key, ty, dc, isType);
    if (!sym) {
        return nullptr;
    }
    if (!curScope_->insert(sym)) {
        delete sym;
        return nullptr;
    }
    return sym;
}
Symbol* SymbolTableContext::insert(Symbol::SymbolType st, const std::string& key, Type* ty, NamedDecl* dc,  bool isType)
{
    return insert(st, key, QualType(ty, 0), dc, isType);
} 

Symbol* SymbolTableContext::insertLabel(const std::string& key, NamedDecl* dc)
{
    return insert(Symbol::LABEL, key, nullptr, dc);
}

Symbol* SymbolTableContext::insertRecord(const std::string& key, QualType ty, NamedDecl* dc)
{
    return insert(Symbol::RECORD, key, ty, dc, true);
}

Symbol* SymbolTableContext::insertMember(const std::string& key, QualType ty, NamedDecl* dc)
{
    return insert(Symbol::MEMBER, key, ty, dc);
}

Symbol* SymbolTableContext::insertNormal(const std::string& key, QualType ty, NamedDecl* dc, bool isType)
{
    return insert(Symbol::NORMAL, key, ty, dc, isType);
}

Symbol* SymbolTableContext::lookup(Symbol::SymbolType st, const std::string& key)
{
    return curScope_->lookup(st, key);
}  

void SymbolTableContext::enterScope(Scope::ScopeType st)
{
    curScope_ = new Scope(st, curScope_);
}

void SymbolTableContext::exitScope()
{
    if (!curScope_) {
        throw std::string("curScope_ is nullptr");
    }
    curScope_ = curScope_->getParent();
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
    Symbol* sym = lookup(Symbol::NORMAL, tk->value_);
    if (sym && sym->isTypeName()) {
        return true;
    }
    return false;
}

bool SymbolTableContext::isTypeQualifier(Token* tk)
{
    if (!tk) {
        return false;
    }
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
    // void
    insert(Symbol::NORMAL, "void", VoidType::NewObj(), nullptr);
    // bool
    insert(Symbol::NORMAL, "_Bool", BoolType::NewObj(), nullptr);
    // char型
    insert(Symbol::NORMAL, "signed_char", IntegerType::NewObj(IntegerType::SIGNED, IntegerType::BYTE, IntegerType::CHAR), nullptr);
    insert(Symbol::NORMAL, "unsignd_char", IntegerType::NewObj(IntegerType::UNSIGNED, IntegerType::BYTE, IntegerType::CHAR), nullptr);
    // 整型
    insert(Symbol::NORMAL, "signed_short_int", IntegerType::NewObj(IntegerType::SIGNED, IntegerType::SHORT, IntegerType::INT), nullptr);
    insert(Symbol::NORMAL, "signed_int", IntegerType::NewObj(IntegerType::SIGNED, IntegerType::NORMAL, IntegerType::INT), nullptr);
    insert(Symbol::NORMAL, "signed_long_int", IntegerType::NewObj(IntegerType::SIGNED, IntegerType::LONG, IntegerType::INT), nullptr);
    insert(Symbol::NORMAL, "signed_long_long_int", IntegerType::NewObj(IntegerType::SIGNED, IntegerType::LONG2, IntegerType::INT), nullptr);
    insert(Symbol::NORMAL, "unsignd_short_int", IntegerType::NewObj(IntegerType::UNSIGNED, IntegerType::SHORT, IntegerType::INT), nullptr);
    insert(Symbol::NORMAL, "unsignd_int", IntegerType::NewObj(IntegerType::UNSIGNED, IntegerType::NORMAL, IntegerType::INT), nullptr);
    insert(Symbol::NORMAL, "unsignd_long_int", IntegerType::NewObj(IntegerType::UNSIGNED, IntegerType::LONG, IntegerType::INT), nullptr);
    insert(Symbol::NORMAL, "unsigned_long_long_int", IntegerType::NewObj(IntegerType::UNSIGNED, IntegerType::LONG2, IntegerType::INT), nullptr);
    // 浮点型
    insert(Symbol::NORMAL, "float", RealFloatingType::NewObj(RealFloatingType::FLOAT), nullptr);
    insert(Symbol::NORMAL, "double", RealFloatingType::NewObj(RealFloatingType::DOUBLE), nullptr);
    insert(Symbol::NORMAL, "long_double", RealFloatingType::NewObj(RealFloatingType::LONG_DOUBLE), nullptr);
    // 复数
    insert(Symbol::NORMAL, "float_complex", ComplexType::NewObj(ComplexType::FLOAT), nullptr);
    insert(Symbol::NORMAL, "double_complex", ComplexType::NewObj(ComplexType::DOUBLE), nullptr);
    insert(Symbol::NORMAL, "long_double_complex", ComplexType::NewObj(ComplexType::LONG_DOUBLE), nullptr);
}

Type* SymbolTableContext::getBuiltTypeByTypeSpec(int tq)
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
    Symbol* sym = lookup(Symbol::NORMAL, key);
    return sym ? sym->getType().getPtr() : nullptr;
}