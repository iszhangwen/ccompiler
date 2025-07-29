/*
类型系统实现:
注解：C99的类型系统分为：
    Type    -空类型(VoidType)
            整型     --->    整数类型 char, short, int, long, long long _Bool
            浮点类型 
            复数类型
            派生类型 
                        --->    浮点类型 folat, double, long double
                        --->    布尔类型 _Bool
            
                --->    空类型   void
    派生类型     --->    指针
                        数组类型
                        结构体和联合体
                        枚举
                        函数
*/
#pragma once
#include <string>
#include <assert.h>
#include "ast.h"

// QualType存储了包装的类型限定符:主要作用了为了规范类型，维护类型系统的稳定性
class QualType 
{
    uint8_t ql_; // 类型限定符
    Type* ty_;  // 存储类型指针
public:
    // 构造函数
    QualType() {}
    QualType(const Type *ptr, unsigned qual=0x00)
        :ty_(ptr) {
        // 校验类型限定符的合法性
        assert((qual & ~TypeQualifier::TQ_MASK) == 0);
        ptr_ |= qual;
    }

    // 获取类型指针
    bool isNull() const {return getPtr() == nullptr;}
    Type *getPtr() {return ty_);}
    
    // 运算符重载: 
    // 隐式类型转换: 用于在布尔上下文环境中表达
    operator bool() const {return !isNull();}
    // 解引用运算符重载
    Type &operator*() {return *getPtr();}
    const Type &operator*() const {return *getPtr();}
    // 箭头运算符重载：箭头运算符是双目运算符
    Type *operator->() {return getPtr();}
    const Type *operator->() const{return getPtr();}
        // 逻辑运算符重载
    friend bool operator==(const QualType &LHS, const QualType &RHS) {
        return LHS.operator->() == RHS.operator->();
    }
    friend bool operator!=(const QualType &LHS, const QualType &RHS) {
        return !(LHS == RHS);
    }

    // 获取是否有类型限定符
    unsigned getQual() const {return ql_ & 0x07;}
    bool isConstQual() const {return ql_ & TypeQualifier::CONST;}
    bool isRestrictQual() const {return ql_ & TypeQualifier::RESTRICT;}
    bool isVolatileQual() const {return ql_ & TypeQualifier::VOLATILE;}
};

// 对类型系统而言：存储限定符是针对变量，类型限定符是针对类型
// 类型系统基类,存储了规范类型
class Type 
{
public:
    enum TypeKind{
    // 空类型
    VOID, 
    // 算术类型
    BOOL, INTEGER, REALFLOATING, COMPLEX,
    // 派生类型：从对象，函数和不完整类型可以构造任意数量派生类型; 数组，函数，指针
    ARRAY, FUNCION, POINTER,
    // 自定义类型
    STRUCT, UNION, ENUM,
    // 类型别名
    TYPEDEF
    };

protected:
    TypeKind kind_;  // 类型域标识
    QualType canonicalType_; // 规范类型
    int align; // 对齐方式
    int size;  // 类型要占用多大空间
    bool isComplete_; // 是否是完整类型
    Type(TypeKind tk, QualType canonical, bool isComplete = false)
    : kind_(tk), canonicalType_(canonical.isNull() ? QualType(this, 0) : canonical), isComplete_(isComplete) {}
    virtual ~Type() {}
  
public:
    // 类型基本属性
    TypeKind getKind() const { return kind_;}
    int getAlign() const {return align;}
    int getSize() const {return size;}
    virtual std::string getName() const {return "";}
    QualType getQualType() {return canonicalType_;}

    // 判断当前类型是否是规范类型
    bool isCanonical() const { return canonicalType_.getPtr() == this; }
        Decl* getDecl() {return nullptr;}
    void setCompleteType(bool complete) {isComplete_ = complete;}

    /* C语言类型分为三种：对象类型，函数类型，不完整类型
    对象类型：能够完整描述对象存储特征的类型(如int, struct, 数组等)，编译器可以确定大小和内存布局。
    函数类型：描述函数参数和返回值类型的抽象类型
    不完整类型：缺少完整定义的类型（未指定大小的数组或前向声明的结构体）*/
    bool isObjectType();
    //bool isFunctionType();
    bool isCompleteType();
    //6.2.5-1
    bool isBoolType(); 
    //6.2.5-4 ~ 6.2.5-7
    bool isSignedIntegerType();
    bool isUnSignedIntegerType();
    // 6.2.5-10
    bool isRealFloatingType();
    // 6.2.5-11
    bool isComplexType();
    bool isFloatingType();
    // 6.2.5-14
    bool isBasicType();
    // 6.2.5-15
    bool isCharacterType();
    // 6.2.5-16
    bool isEnumeratedType();
    // 6.2.5-17
    bool isIntegerType();
    bool isRealType();
    // 6.2.5-18
    bool isArithmeticType();
    // 6.2.5-19.1
    bool isVoidType();
    bool isArrayType();
    bool isStructType();
    bool isUnionType();
    bool isFunctionType();
    bool isPointerType();
    //bool isAtomicType();
    // 6.2.5-21
    bool isScalarType();
    bool isAggregateType();
    // 6.2.5-24
    bool iaDrivedDeclaratorType();
    // typedef单独判断
    bool isTypeDefNameType();
};


/*---------------------------------基本类型----------------------------------------------*/
// 空类型
class VoidType : public Type
{
protected:
    VoidType(){}
public:
    static VoidType* NewObj();
};

// bool类型
class BoolType : public Type
{
protected:
    BoolType(){}
public:
    static BoolType* NewObj();
};
// 整型
class IntegerType : public Type 
{
    bool isSigned_;
    int width_;
    int kind_;
protected:
    IntegerType(int tq);
public:
    static IntegerType* NewObj(int tq);
    int getWidth() const {return width_;}
    int getKind() const {return kind_;}
    std::string getName() const override;
    // 判断是否是整数类型
    bool isSigned() const {return isSigned_;}
    bool isCharacter();
};

// 浮点类型
class RealFloatingType : public Type
{
protected:
    RealFloatingType(){}
public:
    static RealFloatingType* NewObj();
};

// 复数类型
class ComplexType : public Type 
{
public:
    enum Kind {
        FLOAT,
        DOUBLE,
        LONG_DOUBLE
    };
    Kind getKind() const {return kind_;}
private:
    Kind kind_;

protected:
    ComplexType(TypeKind co, QualType derived, Kind kd) :
    Type(co, derived), kind_(kd){}

public:
    static ComplexType* NewObj(TypeKind co, QualType derived, Kind kd);
};

/*---------------------------------派生声明符类型---------------------------------------------------*/
class DerivedType : public Type
{
protected:
    DerivedType(TypeKind co, QualType derived)
    : Type(co, derived){}

};
class PointerType : public DerivedType 
{
protected:
    PointerType(QualType pointee) 
    :DerivedType(Type::POINTER, pointee){}
public:
    static PointerType* NewObj(QualType pointee);
    std::string getName() const {return "";}
    static PointerType* newObj(QualType);
    QualType getPointeeType() const {return canonicalType_;}
    bool isConstPointer() const {return canonicalType_.isConstQual();}
    bool isVolatilePointer() const {return canonicalType_.isVolatileQual();}
    bool isRestrictPointer() const {return canonicalType_.isRestrictQual();}
    bool isPointerType() const {return true;}
    bool isIncompleteType() const {return canonicalType_.getPtr()->isIncompleteType();}
    bool isObjectType() const {return canonicalType_.getPtr()->isObjectType();}
    bool isFunctionType() const {return canonicalType_.getPtr()->isFunctionType();}
    Decl* getDecl() {return canonicalType_.getPtr()->getDecl();}
    // 获取指针指向的类型
    QualType getPointee() const {return canonicalType_.getPtr();}
    // 获取指针类型的规范类型
    QualType getCanonicalType() const {return canonicalType_;}
};

// 数组派生类型
class ArrayType : public DerivedType 
{
public:
    /*
    数组大小修饰符：
    Normal: 标准数组声明(int arr[10])
    Static: C99引入得静态大小修饰符(int arr[static 10], 提示编译器至少需要10个元素)
    Star: yoghurt不完整得数组声明（int arr[*] 可变数组）
    */
private:
    int len_;
    const Expr* lenExpr_;

protected:
    ArrayType(QualType can, int len)
    : DerivedType(Type::ARRAY, can), len_(len) {}
    ArrayType(QualType can, Expr* lenExpr)
    : DerivedType(Type::ARRAY, can), lenExpr_(lenExpr) {}

public:
    static ArrayType* NewObj(QualType can, int len);
    static ArrayType* NewObj(QualType can, Expr* lenExpr);
    void setLen(unsigned len) {len_ = len;}
    unsigned getLen() const {return len_;}
    QualType getElementType() const {return getTy;}
    bool isStarArray() const {return lenExpr_ != nullptr;}
    bool isStaticArray() const {return len_ > 0;}
    bool isNormalArray() const {return len_ > 0 && lenExpr_ == nullptr;}
    bool isArrayType() const {return true;}
    bool isIncompleteType() const {return len_ == 0 && lenExpr_ == nullptr;}
    bool isObjectType() const {return canonicalType_.getPtr()->isObjectType();}
    bool isFunctionType() const {return canonicalType_.getPtr()->isFunctionType();}
    Decl* getDecl() {return canonicalType_.getPtr()->getDecl();}
    // 获取数组元素类型
    QualType getElement() const {return canonicalType_.getPtr();}
    // 获取数组类型的规范类型
    QualType getCanonicalType() const {return canonicalType_;}
};

class FunctionType : public DerivedType {
    bool inline_; // 是否是内联函数
    bool noReturn_; // 是否是无返回值函数
    std::vector<ParmVarDecl*> params_; // 函数参数列表
  
protected:
    FunctionType(QualType qt, bool isInline, bool isNoReturn, std::vector<ParmVarDecl*>& params)
    : DerivedType(Type::FUNCION, qt), inline_(isInline), noReturn_(isNoReturn), params_(params){}

public:
    static FunctionType* NewObj(QualType qt, bool isInline, bool isNoReturn, std::vector<ParmVarDecl*>& params);
    QualType getResultType() const { return QualType(); }
    bool isInline() const {return inline_;}
    bool isNoReturn() const {return noReturn_;}
    const std::vector<ParmVarDecl*>& getParams() const { return params_; }
    void setParams(const std::vector<ParmVarDecl*>& params) { params_ = params; }
    bool isFunctionType() const {return true;}
    bool isIncompleteType() const {return false;}
    bool isObjectType() const {return false;}
    bool isTypeName() const {return true;}
    Decl* getDecl() {return canonicalType_.getPtr()->getDecl();}
    // 获取函数类型的规范类型
    QualType getCanonicalType() const {return canonicalType_;}
    // 获取函数返回值类型
    QualType getReturnType() const {
        if (params_.empty()) {
            return QualType(); // 如果没有参数，返回空类型
        }
        return params_.front()->getQualType(); // 假设第一个参数是返回值类型
    }
    // 获取函数参数类型列表
    std::vector<QualType> getParamTypes() const {
        std::vector<QualType> paramTypes;
        for (const auto& param : params_) {
            paramTypes.push_back(param->getQualType());
        }
        return paramTypes;
    }
};

/*------------------------------结构体和枚举类型----------------------------------------------*/
class TagType : public Type {
    Decl* decl_;
    bool process_;

protected:
    TagType(TypeKind tk, QualType can, Decl* dc)
    : Type(tk, can), decl_(dc){}

public:  
    Decl* getDecl() const {return decl_;}
    void setDecl(Decl* dc) {decl_ = dc;}
    bool isStructType() const {return kind_ == Type::STRUCT;}
    bool isUnionType() const {return kind_ == Type::UNION;}
    bool isEnumType() const {return kind_ == Type::ENUM;}
};

class RecordType : public TagType {
protected:
    RecordType(TypeKind tk, Decl *dc)
    : TagType(tk, QualType(), dc) {}
public:
    static RecordType* NewObj(bool isStruct, Decl *dc);
};

class EnumType : public TagType {
protected:
    EnumType(Decl *dc)
    : TagType(Type::ENUM, QualType(), dc) {}
public:
    static EnumType* NewObj(Decl *dc);
};

/*--------------------------typedef定义的类型别名--------------------------------------*/
class TypedefType : public Type {
    TypedefDecl *decl_;
protected:
    TypedefType(TypedefDecl *decl, QualType qt) 
    : Type(Type::TYPEDEF, qt), decl_(decl) {}

public:
    static TypedefType* NewObj(TypedefDecl *decl, QualType can);
    TypedefDecl *getDecl() const { return decl_; }
    void setDecl(TypedefDecl *decl) { decl_ = decl; }
};