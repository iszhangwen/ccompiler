#include "type.h"
#include "decl.h"

bool Type::isObjectType()
{
    return !isFunctionType();
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
bool Type::isTypeDefType()
{   
    return (Type::TYPEDEF == getKind());
}

VoidType* VoidType::NewObj()
{
    return new VoidType();
}

BoolType* BoolType::NewObj()
{
    return new BoolType();
}

IntegerType* IntegerType::NewObj(Sign sig, Category cate)
{
    return new IntegerType(sig, cate);
}

RealFloatingType* RealFloatingType::NewObj(Category cate)
{
    return new RealFloatingType(cate);
}
ComplexType* ComplexType::NewObj(Category cate)
{
    return new ComplexType(cate);
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

RecordType::RecordType(TagDecl* dc, bool isStruct)
: TagType(isStruct ? Type::STRUCT : Type::UNION, dc)
{
}

RecordType* RecordType::NewObj(TagDecl* dc, bool isStruct)
{
    return new RecordType(dc, isStruct ? Type::STRUCT : Type::UNION);
}

EnumType::EnumType(EnumDecl* dc)
: TagType(Type::ENUM, dc)
{
}

EnumType* EnumType::NewObj(EnumDecl *dc)
{
    return new EnumType(dc);
}
TypedefType* TypedefType::NewObj(QualType qt, TypedefDecl *dc)
{
    return new TypedefType(qt, dc);
}








