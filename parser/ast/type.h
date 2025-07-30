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
    uint8_t ql_; // 类型限定符
    Type* ty_;  // 存储类型指针
public:
    // 构造函数
    QualType() {}
    QualType(Type *ptr, unsigned qual=0x00)
        :ty_(ptr) {
        // 校验类型限定符的合法性
        assert((qual & ~TypeQualifier::TQ_MASK) == 0);
        ql_ |= qual;
    }

    // 获取类型指针
    bool isNull() const {return (nullptr == getPtr());}
    Type *getPtr() {return ty_;}
    const Type *getPtr() const {return ty_;}
    
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
private:
    TypeKind kind_;  // 类型域标识
    QualType canonicalType_; // 规范类型
    std::string name_;
    int align_; // 对齐方式，32位系统默认4字节对齐
    int size_;  // 类型要占用多大空间
    bool isComplete_; // 是否是完整类型
    
protected:
    Type(TypeKind tk, QualType canonical, const std::string& name, int size, bool isComplete = false)
    : kind_(tk), canonicalType_(canonical.isNull() ? QualType(this, 0) : canonical), name_(name), size_(size), isComplete_(isComplete) {}
    Type(TypeKind tk, QualType canonical, bool isComplete = false)
    : Type(tk, canonical, "", 0, isComplete) {}
    virtual ~Type() {}
  
public:
    // 类型基本属性
    TypeKind getKind() const { return kind_;}
    int getAlign() const {return align_;}
    void setAlign(int align) {align_ = align;}
    int getSize() const {return size_;}
    void setSize(int size) {size_ = size;}
    std::string getName() const {return name_;}
    void getName(const std::string& name) {name_ = name;}
    QualType getQualType() {return canonicalType_;}
    const QualType getQualType() const {return canonicalType_;}
    void setQualType(QualType ql) {canonicalType_ = ql;}
    // 判断当前类型是否是规范类型
    bool isCanonical() { return canonicalType_.getPtr() == this; }

    /* C语言类型分为三种：对象类型，函数类型，不完整类型
    对象类型：能够完整描述对象存储特征的类型(如int, struct, 数组等)，编译器可以确定大小和内存布局。
    函数类型：描述函数参数和返回值类型的抽象类型
    不完整类型：缺少完整定义的类型（未指定大小的数组或前向声明的结构体）*/
    bool isObjectType();
    //bool isFunctionType();
    bool isCompleteType();
    void setCompleteType(bool complete) {isComplete_ = complete;}
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
    bool isTypeDefType();
};

/*---------------------------------基本类型----------------------------------------------*/
// 空类型
class VoidType : public Type
{
protected:
    VoidType()
    : Type(Type::VOID, QualType(this), "void", 0, false){}
public:
    static VoidType* NewObj();
};

// bool类型
class BoolType : public Type
{
protected:
    BoolType()
    : Type(Type::BOOL, QualType(this), "_Bool", 1, true){}
public:
    static BoolType* NewObj();
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
    enum Category {
        CHAR,
        SHORT,
        INT,
        LONG,
        LONG2
    };
private:
    Sign signed_;
    Category category_;
protected:
    IntegerType(Sign sig, Category cate)
    : Type(Type::INTEGER, QualType(this), true), signed_(sig), category_(cate) {}
public:
    static IntegerType* NewObj(Sign sig, Category cate);
    // 判断是否是整数类型
    bool isSigned() const {return (Sign::SIGNED == signed_);}
    bool isUnSigned() const {return (Sign::UNSIGNED == signed_);}
    bool isChar() const {return (Category::CHAR == category_);}
    bool isShort() const {return (Category::SHORT == category_);}
    bool isInt() const {return (Category::INT == category_);}
    bool isLong() const {return (Category::LONG == category_);}
    bool isLongLong() const {return (Category::LONG2 == category_);}
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
private:
    Category category_;
protected:
    RealFloatingType(Category cate)
    : Type(Type::REALFLOATING, QualType(this), true), category_(cate) {}
public:
    static RealFloatingType* NewObj(Category cate);
    bool isFloat() const {return (Category::FLOAT == category_);}
    bool isDouble() const {return (Category::DOUBLE == category_);}
    bool isLongDouble() const {return (Category::LONG_DOUBLE == category_);}
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
private:
    Category category_;
protected:
    ComplexType(Category cate) 
    : Type(Type::COMPLEX, QualType(this), true), category_(cate) {}
public:
    static ComplexType* NewObj(Category cate);
    bool isFloatComplex() const {return (Category::FLOAT == category_);}
    bool isDoubleComplex() const {return (Category::DOUBLE == category_);}
    bool isLongDoubleComplex() const {return (Category::LONG_DOUBLE == category_);}
};

/*---------------------------------派生声明符类型---------------------------------------------------*/
class PointerType : public Type 
{
protected:
    PointerType(QualType pointee) 
    : Type(Type::POINTER, pointee, true){}
public:
    static PointerType* NewObj(QualType pointee);
    QualType getPointeeType() const {return getQualType();}
    bool isConstPointer() const {return getQualType().isConstQual();}
    bool isVolatilePointer() const {return getQualType().isVolatileQual();}
    bool isRestrictPointer() const {return getQualType().isRestrictQual();}
    // 获取指针指向的类型
    // 获取指针类型的规范类型
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
private:
    int len_;
    Expr* lenExpr_;

protected:
    ArrayType(QualType elem, int len)
    : Type(Type::ARRAY, elem, true), len_(len) {}
    ArrayType(QualType elem, Expr* lenExpr)
    : Type(Type::ARRAY, elem, true), lenExpr_(lenExpr) {}

public:
    static ArrayType* NewObj(QualType can, int len);
    static ArrayType* NewObj(QualType can, Expr* lenExpr);
    void setLen(unsigned len) {len_ = len;}
    int getLen() const {return len_;}
    void setLenExpr(Expr* ex) {lenExpr_ = ex;}
    Expr* getLenExpr() const {return lenExpr_;}

    bool isStarArray() const {return lenExpr_ != nullptr;}
    bool isStaticArray() const {return len_ > 0;}
    bool isNormalArray() const {return len_ > 0 && lenExpr_ == nullptr;}
};

class FunctionType : public Type {
    bool isInline_; // 是否是内联函数
    bool isNoReturn_; // 是否是无返回值函数
    std::vector<ParmVarDecl*> proto_; // 函数参数列表
  
protected:
    FunctionType(QualType qt, bool isInline, bool isNoReturn, std::vector<ParmVarDecl*>& proto)
    : Type(Type::FUNCION, qt, false), isInline_(isInline), isNoReturn_(isNoReturn), proto_(proto){}

public:
    static FunctionType* NewObj(QualType qt, bool isInline, bool isNoReturn, std::vector<ParmVarDecl*>& proto);

    QualType getResultType() const { return QualType(); }
    bool isInline() const {return isInline_;}
    bool isNoReturn() const {return isNoReturn_;}
    const std::vector<ParmVarDecl*>& getParams() const { return proto_;}
    void setParams(const std::vector<ParmVarDecl*>& params) { proto_ = params; }
};

/*------------------------------结构体和枚举类型----------------------------------------------*/
class TagType : public Type {
    TagDecl* tag_;
protected:
    TagType(TypeKind tk, TagDecl* dc)
    : Type(tk,  QualType(this)), tag_(dc){}

public:  
    TagDecl* getTagDecl() const {return tag_;}
    void setTagDecl(TagDecl* dc) {tag_ = dc;}
};

class RecordType : public TagType {
protected:
    RecordType(TagDecl* dc, bool isStruct);
public:
    static RecordType* NewObj(TagDecl* dc, bool isStruct);
};

class EnumType : public TagType {
protected:
    EnumType(EnumDecl* dc);
public:
    static EnumType* NewObj(EnumDecl* dc);
};

/*--------------------------typedef定义的类型别名--------------------------------------*/
class TypedefType : public Type {
private:
    TypedefDecl *decl_;
protected:
    TypedefType(QualType qt, TypedefDecl *decl) 
    : Type(Type::TYPEDEF, qt), decl_(decl) {}

public:
    static TypedefType* NewObj(QualType qt, TypedefDecl *decl);
    TypedefDecl *getDecl() const { return decl_; }
    void setDecl(TypedefDecl *dc) { decl_ = dc; }
};