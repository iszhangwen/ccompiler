#include "type.h"

bool Type::isObjectType()
{

}
//bool isFunctionType();
bool Type::isCompleteType()
{
    return isComplete_;
}
//6.2.5-1
bool Type::isBoolType()
{
    return (Type::BOOL == getKind());
}
//6.2.5-4 ~ 6.2.5-7
bool Type::isSignedIntegerType()
{
    if (getKind() == Type::INTEGER) {
        return dynamic_cast<IntegerType*>(this)->isSigned();
    }
    return false;
}
bool Type::isUnSignedIntegerType()
{
    if (getKind() == Type::INTEGER) {
        return !dynamic_cast<IntegerType*>(this)->isSigned();
    }
    return false;
}
// 6.2.5-10
bool Type::isRealFloatingType()
{
    return (Type::REALFLOATING == getKind());
}
// 6.2.5-11
bool Type::isComplexType()
{
    return (Type::COMPLEX == getKind());
}
bool Type::isFloatingType()
{
    return (isRealFloatingType() || isComplexType());
}
// 6.2.5-14
bool Type::isBasicType()
{
    return (isSignedIntegerType() 
    || isUnSignedIntegerType()
    || isFloatingType());
}
// 6.2.5-15
bool Type::isCharacterType()
{
    if (Type::INTEGER == getKind()) {
        return dynamic_cast<IntegerType*>(this)->isCharacter();
    }
    return false;
}
// 6.2.5-16
bool Type::isEnumeratedType()
{
    return (Type::ENUM == getKind());
}
// 6.2.5-17
bool Type::isIntegerType()
{
    return (isSignedIntegerType() || isUnSignedIntegerType() || isEnumeratedType());
}
bool Type::isRealType()
{
    return (isIntegerType() || isRealFloatingType());
}
// 6.2.5-18
bool Type::isArithmeticType()
{
    return (isIntegerType() || isFloatingType());
}
// 6.2.5-19.1
bool Type::isVoidType()
{
    return (Type::VOID == getKind());
}
bool Type::isArrayType()
{
    return (Type::ARRAY == getKind());
}
bool Type::isStructType()
{
    return (Type::STRUCT == getKind());
}
bool Type::isUnionType()
{
    return (Type::UNION == getKind());
}
bool Type::isFunctionType()
{
    return (Type::FUNCION == getKind());
}
bool Type::isPointerType()
{
    return (Type::POINTER == getKind());
}
// 6.2.5-21
bool Type::isScalarType()
{
    return (isArithmeticType() || isPointerType());
}
bool Type::isAggregateType()
{
    return (isArrayType() || isStructType());
}
// 6.2.5-24
bool Type::iaDrivedDeclaratorType()
{
    return (isArrayType() || isFunctionType() || isPointerType());
}
// typedef单独判断
bool Type::isTypeDefNameType()
{   
    return (Type::TYPEDEF == getKind());
}

VoidType* VoidType::NewObj()
{
    return new VoidType();
}

IntegerType::IntegerType(int ts)
: Type(Type::INTEGER, QualType())
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

IntegerType* IntegerType::NewObj(int tq)
{
    return new IntegerType(tq);
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








