#include "type.h"
#include "decl.h"

bool Type::isObjectType()
{
    return !isFunctionType();
}
//bool isFunctionType();
bool Type::isCompleteType()
{
    return m_isComplete;
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
        return dynamic_cast<IntegerType*>(this)->isChar();
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
    return false;
}
bool Type::isUnionType()
{
    return false;
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
bool Type::isTypeDefType()
{   
    return (Type::TYPEDEF == getKind());
}








