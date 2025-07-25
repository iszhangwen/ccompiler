#include "type.h"

BuiltinType::BuiltinType(int ts)
: Type(Type::BUILTIN, QualType())
{
    // 解析类别
    kind_ = (ts & TypeSpecifier::VOID) 
    | (ts & TypeSpecifier::_BOOL) 
    | (ts & TypeSpecifier::CHAR) 
    | (ts & TypeSpecifier::INT) 
    | (ts & TypeSpecifier::FLOAT) 
    | (ts & TypeSpecifier::DOUBLE)
    | (ts & TypeSpecifier::LONGDOUBLE);
    // 解析符号
    isSigned_ = true;
    if (ts & TypeSpecifier::UNSIGNED) {
        isSigned_ = false;
    }
    // 解析长度
    width_ = (ts & TypeSpecifier::SHORT)
    | (ts & TypeSpecifier::LONG)
    | (ts & TypeSpecifier::LONGLONG);
}

BuiltinType* BuiltinType::NewObj(int tq)
{
    return new BuiltinType(tq);
}
ComplexType* ComplexType::NewObj(TypeKind co, QualType derived, Kind kd)
{
    return new ComplexType(co, derived, kd);
}
PointerType* PointerType::NewObj(QualType pointee)
{
    return new PointerType(pointee);
}
ArrayType* ArrayType::NewObj(QualType qt, int len)
{
    return new ArrayType(qt, len);
}
ArrayType* ArrayType::NewObj(QualType qt, Expr* lenExpr)
{
    return new ArrayType(qt, lenExpr);
}
FunctionType* FunctionType::NewObj(QualType qt, bool isInline, bool isNoReturn, std::vector<ParmVarDecl*>& params)
{
    return new FunctionType(qt, isInline, isNoReturn, params);
}
RecordType* RecordType::NewObj(bool isStruct, Decl *dc)
{
    TypeKind tk = isStruct ? Type::STRUCT : Type::UNION;
    return new RecordType(tk, dc);
}
EnumType* EnumType::NewObj(Decl *dc)
{
    return new EnumType(dc);
}
TypedefType* TypedefType::NewObj(TypedefDecl *dc, QualType qt)
{
    return new TypedefType(dc, qt );
}







