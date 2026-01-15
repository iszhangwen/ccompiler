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

class Type;
// QualType存储了包装的类型限定符:主要作用了为了规范类型，维护类型系统的稳定性
class QualType 
{
public:
    // 构造函数
    QualType() {}
    QualType(std::shared_ptr<Type> ptr, unsigned qual=0x00)
        :m_type(ptr) {
        // 校验类型限定符的合法性
        assert((qual & ~TypeQualifier::TQ_MASK) == 0);
        m_qualSpec |= qual;
    }

    // 获取类型指针
    bool isNull() const {return (nullptr == getPtr());}
    std::shared_ptr<Type> getPtr() {return m_type;}
    const std::shared_ptr<Type> getPtr() const {return m_type;}
    
    // 运算符重载: 
    // 隐式类型转换: 用于在布尔上下文环境中表达
    operator bool() const {return !isNull();}
    // 解引用运算符重载
    Type &operator*() {return *getPtr();}
    const Type &operator*() const {return *getPtr();}
    
    // 箭头运算符重载：箭头运算符是双目运算符
    std::shared_ptr<Type> operator->() {return getPtr();}
    const std::shared_ptr<Type> operator->() const{return getPtr();}
    // 逻辑运算符重载
    friend bool operator==(const QualType &LHS, const QualType &RHS) {return LHS.operator->() == RHS.operator->();}
    friend bool operator!=(const QualType &LHS, const QualType &RHS) {return !(LHS == RHS);}

    // 获取是否有类型限定符
    unsigned getQual() const {return m_qualSpec & 0x07;}
    bool isConstQual() const {return m_qualSpec & TypeQualifier::CONST;}
    bool isRestrictQual() const {return m_qualSpec & TypeQualifier::RESTRICT;}
    bool isVolatileQual() const {return m_qualSpec & TypeQualifier::VOLATILE;}

private:
    uint8_t m_qualSpec; // 类型限定符
    std::shared_ptr<Type> m_type;  // 存储类型指针
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
    RECORD, ENUM,
    // 类型别名
    TYPEDEF
    };
     
public:
    Type(TypeKind tk, QualType canonical, bool isComplete = false){}
    virtual ~Type() {}
    // 类型基本属性

    TypeKind getKind() const { return m_kind;}
    int getAlign() const {return m_align;}
    void setAlign(int align) {m_align = align;}
    int getSize() const {return m_size;}
    void setSize(int size) {m_size = size;}
    std::string getName() const {return m_name;}
    void setName(const std::string& name) {m_name = name;}
    QualType getQualType() {return m_qualType;}
    const QualType getQualType() const {return m_qualType;}
    void setQualType(QualType ql) {m_qualType = ql;}
    // 判断当前类型是否是规范类型
    bool isCanonical() { return m_qualType.getPtr().get() == this; }

    /* C语言类型分为三种：对象类型，函数类型，不完整类型
    对象类型：能够完整描述对象存储特征的类型(如int, struct, 数组等)，编译器可以确定大小和内存布局。
    函数类型：描述函数参数和返回值类型的抽象类型
    不完整类型：缺少完整定义的类型（未指定大小的数组或前向声明的结构体）*/
    virtual bool isObjectType();
    //bool isFunctionType();
    virtual bool isCompleteType();
    void setCompleteType(bool complete) {m_isComplete = complete;}
    //6.2.5-1
    virtual bool isBoolType();
    //6.2.5-4 ~ 6.2.5-7
    virtual bool isSignedIntegerType();
    virtual bool isUnSignedIntegerType();
    // 6.2.5-10
    virtual bool isRealFloatingType();
    // 6.2.5-11
    virtual bool isComplexType();
    virtual bool isFloatingType();
    // 6.2.5-14
    virtual bool isBasicType();
    // 6.2.5-15
    virtual bool isCharacterType();
    // 6.2.5-16
    virtual bool isEnumeratedType();
    // 6.2.5-17
    virtual bool isIntegerType();
    virtual bool isRealType();
    // 6.2.5-18
    virtual bool isArithmeticType();
    // 6.2.5-19.1
    virtual bool isVoidType();
    virtual bool isArrayType();
    virtual bool isStructType();
    virtual bool isUnionType();
    virtual bool isFunctionType();
    virtual bool isPointerType();
    //bool isAtomicType();
    // 6.2.5-21
    virtual bool isScalarType();
    virtual bool isAggregateType();
    // 6.2.5-24
    virtual bool iaDrivedDeclaratorType();
    // typedef单独判断
    virtual bool isTypeDefType();

private:
    TypeKind m_kind;  // 类型域标识
    QualType m_qualType; // 规范类型
    std::string m_name;
    int m_align; // 对齐方式，32位系统默认4字节对齐
    int m_size;  // 类型要占用多大空间
    bool m_isComplete; // 是否是完整类型
};

// 空类型
class VoidType : public Type
{
public:
    VoidType()
    : Type(Type::VOID, QualType(),false){
        setName("void");
        setSize(0);
    }
};

// bool类型
class BoolType : public Type
{
public:
    BoolType()
    : Type(Type::BOOL, QualType(), true){
        setName("_Bool");
        setSize(1);
    }

};
// 整型
class IntegerType : public Type 
{
public:
    enum Sign {
        NOSIGN,
        SIGNED,
        UNSIGNED
    };

    enum Width {
        BYTE,
        SHORT,
        NORMAL,
        LONG,
        LONG2
    };

    enum Category {
        CHAR,
        INT,
    };

    IntegerType(Sign sig, Width wid, Category cate)
    : Type(Type::INTEGER, QualType(), true)
    , m_signed(sig)
    , m_width(wid)
    , m_category(cate) {}

    // 判断是否是整数类型
    bool isSigned() const {return (Sign::SIGNED == m_signed);}
    bool isUnSigned() const {return (Sign::UNSIGNED == m_signed);}
    bool isShort() const {return (Width::SHORT == m_width);}
    bool isNormal() const {return (Width::NORMAL == m_width);}
    bool isLong() const {return (Width::LONG == m_width);}
    bool isLongLong() const {return (Width::LONG2 == m_width);}
    bool isChar() const {return (Category::CHAR == m_category);}
    bool isInt() const {return (Category::INT == m_category);}

private:
    Sign m_signed;
    Width m_width;
    Category m_category;
};

// 浮点类型
class RealFloatingType : public Type
{
public:
    enum Category{
        FLOAT,
        DOUBLE,
        LONG_DOUBLE
    };
    RealFloatingType(Category cate)
    : Type(Type::REALFLOATING, QualType(), true), m_category(cate) {}

    bool isFloat() const {return (Category::FLOAT == m_category);}
    bool isDouble() const {return (Category::DOUBLE == m_category);}
    bool isLongDouble() const {return (Category::LONG_DOUBLE == m_category);}

private:
    Category m_category;
};

// 复数类型
class ComplexType : public Type 
{
public:
    enum Category {
        FLOAT,
        DOUBLE,
        LONG_DOUBLE
    };

    ComplexType(Category cate) 
    : Type(Type::COMPLEX, QualType(), true), m_category(cate) {}

    bool isFloatComplex() const {return (Category::FLOAT == m_category);}
    bool isDoubleComplex() const {return (Category::DOUBLE == m_category);}
    bool isLongDoubleComplex() const {return (Category::LONG_DOUBLE == m_category);}

private:
    Category m_category;
};

/*派生声明符类型*/
class PointerType : public Type 
{
public:
    PointerType(QualType pointee) 
    : Type(Type::POINTER, pointee, true){}

    QualType getPointeeType() const {return getQualType();}
    bool isConstPointer() const {return getQualType().isConstQual();}
    bool isVolatilePointer() const {return getQualType().isVolatileQual();}
    bool isRestrictPointer() const {return getQualType().isRestrictQual();}
};

// 数组派生类型
class ArrayType : public Type 
{
public:
    /*
    数组大小修饰符：
    Normal: 标准数组声明(int arr[10])
    Static: C99引入得静态大小修饰符(int arr[static 10], 提示编译器至少需要10个元素)
    Star: yoghurt不完整得数组声明（int arr[*] 可变数组）
    */
    ArrayType()
    : Type(Type::ARRAY, QualType(), true), m_len(0) {}
    ArrayType(QualType elem, int len)
    : Type(Type::ARRAY, elem, true), m_len(len) {}
    ArrayType(QualType elem, std::shared_ptr<Expr> lenExpr)
    : Type(Type::ARRAY, elem, true), m_lenExpr(lenExpr) {}

    void setLen(unsigned len) {m_len = len;}
    int getLen() const {return m_len;}
    void setLenExpr(std::shared_ptr<Expr> ex) {m_lenExpr = ex;}
    std::shared_ptr<Expr> getLenExpr() const {return m_lenExpr;}

    bool isStarArray() const {return m_lenExpr != nullptr;}
    bool isStaticArray() const {return m_len > 0;}
    bool isNormalArray() const {return m_len > 0 && m_lenExpr == nullptr;}

private:
    int m_len;
    std::shared_ptr<Expr> m_lenExpr;
};

class FunctionType : public Type
{
public:
    FunctionType()
    : Type(Type::FUNCION, QualType(), false), m_isInline(false), m_isNoReturn(false){}

    bool isInline() {return m_isInline;}
    void setIsInline(bool flag) {m_isInline = flag;}
    bool isNoReturn() {return m_isNoReturn;}
    void setIsNotReturn(bool flag) {m_isNoReturn = flag;}
    std::vector<QualType> getParams() {return m_paramTypes;}
    void setParams(std::vector<QualType> params) {m_paramTypes = params;}

private:
    bool m_isInline; // 是否是内联函数
    bool m_isNoReturn; // 是否是无返回值函数
    std::vector<QualType> m_paramTypes; // 函数参数列表
};

class RecordType : public Type
{
public:
    RecordType(bool isStruct = true)
    : Type(Type::RECORD,  QualType()), m_isStruct(isStruct), m_decl(nullptr) {
        setCompleteType(false);
    }

    std::shared_ptr<RecordDecl> getDecl() {return m_decl;}
    void setDecl(std::shared_ptr<RecordDecl> dc) {m_decl = dc;}

    bool isStruct() const {return m_isStruct;}
    void setIsStruct(bool val) {m_isStruct = val;}

    bool isCompleteType() {return m_decl != nullptr;}

private:
    bool m_isStruct;
    std::shared_ptr<RecordDecl> m_decl;
};

class EnumType : public Type
{
public:
    EnumType()
    : Type(Type::ENUM, QualType()), m_decl(nullptr) {
        setCompleteType(false);
    }

    std::shared_ptr<EnumDecl> getDecl() {return m_decl;}
    void setDecl(std::shared_ptr<EnumDecl> dc) {m_decl = dc;}

    bool isCompleteType() {return m_decl != nullptr;}
private:
    std::shared_ptr<EnumDecl> m_decl;
};

class TypedefType : public Type
{
public:
    TypedefType(QualType qt)
    : Type(Type::TYPEDEF, qt), m_decl(nullptr) {}

    std::shared_ptr<TypedefDecl> getDecl() {return m_decl;}
    void setDecl(std::shared_ptr<TypedefDecl> dc) {m_decl = dc;}

    bool isCompleteType() {return m_decl != nullptr;}
private:
    std::shared_ptr<TypedefDecl> m_decl;
};
