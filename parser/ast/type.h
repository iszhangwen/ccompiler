/*
类型系统实现:
注解：C99的类型系统分为：
    基本类型     --->    整数类型 char, short, int, long, long long
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

// 前置类型声明
class Expr;
class Type;
class Decl;
class TagDecl;
class RecordDecl;
class EnumDecl;
class ParmVarDecl;
class TypedefDecl;

// QualType存储了包装的类型限定符:主要作用了为了规范类型，维护类型系统的稳定性
class QualType 
{
    intptr_t ptr_; // 存储类型指针
public:
    // 构造函数
    QualType() {}
    QualType(const Type *ptr, unsigned qual=0x00)
        :ptr_(reinterpret_cast<intptr_t>(ptr)) {
        // 校验类型限定符的合法性
        assert((qual & ~TypeQualifier::TQ_MASK) == 0);
        ptr_ |= qual;
    }

    // 获取类型指针
    bool isNull() const {return getPtr() == nullptr;}
    Type *getPtr() {return reinterpret_cast<Type*>(ptr_ & ~TypeQualifier::TQ_MASK);}
    const Type *getPtr() const {return reinterpret_cast<const Type*>(ptr_ & ~TypeQualifier::TQ_MASK);}
    
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
    unsigned getQual() const {return ptr_ & 0x07;}
    bool isConstQual() const {return ptr_ & TypeQualifier::CONST;}
    bool isRestrictQual() const {return ptr_ & TypeQualifier::RESTRICT;}
    bool isVolatileQual() const {return ptr_ & TypeQualifier::VOLATILE;}
};

// 对类型系统而言：存储限定符是针对变量，类型限定符是针对类型
// 类型系统基类,存储了规范类型
class Type 
{
public:
    enum TypeKind{
    VOID, // 空类型 属于不完整类
    BUILTIN, // 算术类型
    // 复杂类型：float_Complex, double_Complex, long double_Complex
    FLOAT_COMPLEX,
    DOUBLE_COMPLEX,
    LONGDOUBLE_COMPLEX,
    RECORD,
    STRUCT,
    UNION,
    ENUM,
    // 派生类型：从对象，函数和不完整类型可以构造任意数量派生类型; 数组，函数，指针
    ARRAY, // 数组类型从元素类型派生，数组的特征是元素类型和元素数量
    FUNCION, // 函数类型派生自他的返回值类型
    POINTER,
    TYPEDEF
    };
private: 
    TypeKind kind_;  // 类型域标识
    QualType canonicalType_; // 规范类型
    int align; // 对齐方式
    int size;  // 类型要占用多大空间
    bool complete_; // 是否是完整类型

protected:
    Type(TypeKind tk, QualType canonical)
    : kind_(tk), canonicalType_(canonical.isNull() ? QualType(this, 0) : canonical) {}
    virtual ~Type() {}
  
public:
    // 类型基本属性
    TypeKind getKind() const { return kind_;}
    int getAlign() const {return align;}
    int getSize() const {return size;}
    virtual std::string getName() const {return "";}

    // 判断当前类型是否是规范类型
    bool isCanonical() const { return canonicalType_.getPtr() == this; }

    /* C语言类型分为三种：对象类型，函数类型，不完整类型
    对象类型：能够完整描述对象存储特征的类型(如int, struct, 数组等)，编译器可以确定大小和内存布局。
    函数类型：描述函数参数和返回值类型的抽象类型
    不完整类型：缺少完整定义的类型（未指定大小的数组或前向声明的结构体）
    */
    bool isObjectType() const {return false;}
    bool isIncompleteType() const {return false;}
    bool isFunctionType() const {return false;}

    // 判断是否是可变类型：（1）变长数组 （2）基于变长数组的符合类型 Note:注意区分可变数组和不完整类型
    bool isVariablyModifiedType() const;

    // 判断是否是类型定义
    bool isTypeName() const {return true;}
    // 获取类型关联的Decl
    Decl* getDecl() {return nullptr;}

    // 判断类别
    bool isStruct() const {return kind_ == TypeKind::STRUCT;}
    bool isUnion() const {return kind_ == TypeKind::UNION;}
};

// 内建基本类型
/*---------------------------------基本类型----------------------------------------------*/
class BuiltinType : public Type 
{
    bool isSigned_;
    int width_;
    int kind_;
protected:
    BuiltinType(int tq);
public:
    static BuiltinType* NewObj(int tq);
};

// 复数类型
class ComplexType : public Type 
{
public:
    enum Kind {

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

/*---------------------------------派生类型---------------------------------------------------*/
/*
派生类型的实现：
*/
class DerivedType : public Type
{
protected:
    DerivedType(TypeKind co, QualType derived)
    : Type(co, derived){}
};

// 指针类型
// 指针类型的标识方法未 cont int* const*
/*
*/
class PointerType : public DerivedType 
{
protected:
    PointerType(QualType pointee) 
    :DerivedType(Type::POINTER, pointee){}
public:
    static PointerType* NewObj(QualType pointee);
    std::string getName() const {return "";}
    static PointerType* newObj(QualType);
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
};

class FunctionType : public DerivedType {
    bool inline_;
    bool noReturn_;
    std::vector<ParmVarDecl*> params_;
  
protected:
    FunctionType(QualType qt, bool isInline, bool isNoReturn, std::vector<ParmVarDecl*>& params)
    : DerivedType(Type::FUNCION, qt), inline_(isInline), noReturn_(isNoReturn), params_(params){}

public:
    static FunctionType* NewObj(QualType qt, bool isInline, bool isNoReturn, std::vector<ParmVarDecl*>& params);
    QualType getResultType() const { return QualType(); }
    bool isInline() const {return inline_;}
    bool isNoReturn() const {return noReturn_;}
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
};