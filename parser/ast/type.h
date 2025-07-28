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
    // 判断是否是对象类型
    bool isObjectType() const {return kind_ == TypeKind::BUILTIN ||
        kind_ == TypeKind::RECORD || kind_ == TypeKind::STRUCT ||
        kind_ == TypeKind::UNION || kind_ == TypeKind::ENUM ||
        kind_ == TypeKind::ARRAY || kind_ == TypeKind::POINTER ||
        kind_ == TypeKind::TYPEDEF;}
    // 判断是否是函数类型
    bool isFunctionType() const {return kind_ == TypeKind::FUNCION;}
    // 判断是否是完整类型
    bool isCompleteType() const {return complete_;}

    // 判断是否是可变类型：（1）变长数组 （2）基于变长数组的符合类型 Note:注意区分可变数组和不完整类型
    bool isIncompleteType() const {return kind_ == TypeKind::ARRAY && canonicalType_.getPtr()->isIncompleteType();}
    // 判断是否是类型别名
    bool isTypedefType() const {return kind_ == TypeKind::TYPEDEF;}
    // 判断是否是结构体或联合体类型
    bool isStructOrUnionType() const {return kind_ == TypeKind::STRUCT ||
        kind_ == TypeKind::UNION || kind_ == TypeKind::RECORD;}
    // 判断是否是枚举类型
    bool isEnumType() const {return kind_ == TypeKind::ENUM;}
    // 判断是否是内建类型
    bool isBuiltinType() const {return kind_ == TypeKind::BUILTIN ||
        kind_ == TypeKind::FLOAT_COMPLEX || kind_ == TypeKind::DOUBLE_COMPLEX ||
        kind_ == TypeKind::LONGDOUBLE_COMPLEX;}

    // 判断是否是派生类型
    bool isDerivedType() const {return kind_ == TypeKind::ARRAY ||
        kind_ == TypeKind::FUNCION || kind_ == TypeKind::POINTER;}
    // 判断是否是指针类型
    bool isPointerType() const {return kind_ == TypeKind::POINTER;}
    // 判断是否是数组类型
    bool isArrayType() const {return kind_ == TypeKind::ARRAY;}
    // 判断是否是函数类型
    bool isFunctionType() const {return kind_ == TypeKind::FUNCION;}
    // 判断是否是结构体或联合体类型
    bool isRecordType() const {return kind_ == TypeKind::RECORD || kind_ == TypeKind::STRUCT ||
        kind_ == TypeKind::UNION;}
    // 判断是否是枚举类型
    bool isEnumType() const {return kind_ == TypeKind::ENUM;}

    // 判断是否是类型定义
    bool isTypeName() const {return true;}
    // 获取类型关联的Decl
    Decl* getDecl() {return nullptr;}

    // 判断是否使完整类型
    bool isCompleteType() const {return complete_;}
    // 设置是否是完整类型
    void setCompleteType(bool complete) {complete_ = complete;}
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
    bool isSigned() const {return isSigned_;}
    int getWidth() const {return width_;}
    int getKind() const {return kind_;}
    std::string getName() const override;
    // 判断是否是整数类型
    bool isIntegerType() const {return kind_ == Type::BUILTIN && (width_ == 8 || width_ == 16 || width_ == 32 || width_ == 64);}
    // 判断是否是浮点类型
    bool isFloatingType() const {return kind_ == Type::BUILTIN && (     width_ == 32 || width_ == 64 || width_ == 128);}
    // 判断是否是复数类型
    bool isComplexType() const {return kind_ == Type::FLOAT_COMPLEX || kind_ == Type::DOUBLE_COMPLEX || kind_ == Type::LONGDOUBLE_COMPLEX;}
    // 判断是否是布尔类型
    bool isBoolType() const {return kind_ == Type::BUILTIN && width_ == 8;}
    // 判断是否是空类型
    bool isVoidType() const {return kind_ == Type::BUILTIN && width_ == 0;}
    // 判断是否是字符类型
    bool isCharType() const {return kind_ == Type::BUILTIN && width_ == 8;}
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
    QualType getElementType() const {return canonicalType_.getPtr();}
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
    bool isRecordType() const {return kind_ == Type::RECORD;}
    bool isTagType() const {return kind_ == Type::STRUCT || kind_ == Type::UNION || kind_ == Type::ENUM || kind_ == Type::RECORD;}
    bool isIncompleteType() const {return !process_;}
    void setProcess(bool process) {process_ = process;}
    bool isObjectType() const {return true;} // 结构体和联合体是对象类型
    bool isFunctionType() const {return false;} // 结构体和联合体不是函数类型
    bool isTypeName() const {return true;} // 结构体和联合体是类型名
    // 获取结构体/联合体/枚举类型的规范类型
    QualType getCanonicalType() const {return canonicalType_;}
    // 获取结构体/联合体/枚举类型的名称
    std::string getName() const {
        if (decl_) {
            return decl_->getSymbol()->getName();
        }
        return "";
    }
    // 获取结构体/联合体/枚举类型的大小
    int getSize() const {
        if (decl_) {
            return decl_->getSymbol()->getType()->getSize();
        }
        return 0; // 如果没有声明，返回0
    }
    // 获取结构体/联合体/枚举类型的对齐方式
    int getAlign() const {
        if (decl_) {
            return decl_->getSymbol()->getType()->getAlign();
        }
        return 1; // 如果没有声明，返回1
    }
    // 获取结构体/联合体/枚举类型的字段数量 
};

class RecordType : public TagType {
protected:
    RecordType(TypeKind tk, Decl *dc)
    : TagType(tk, QualType(), dc) {}
public:
    static RecordType* NewObj(bool isStruct, Decl *dc);
    static RecordType* NewObj(bool isStruct, QualType can, Decl *dc) {
        return new RecordType(isStruct ? Type::STRUCT : Type::UNION, dc);
    }
    bool isStructType() const {return kind_ == Type::STRUCT;}
    bool isUnionType() const {return kind_ == Type::UNION;}
    bool isRecordType() const {return kind_ == Type::RECORD;}
    bool isTagType() const {return kind_ == Type::STRUCT || kind_ == Type::UNION || kind_ == Type::RECORD;}
    bool isIncompleteType() const {return !process_;}
    bool isObjectType() const {return true;} // 结构体和联合体是对象类型
    bool isFunctionType() const {return false;} // 结构体和联合体不是函数类型
    bool isTypeName() const {return true;} // 结构体和联合体是类型名
    // 获取结构体/联合体/枚举类型的规范类型
    QualType getCanonicalType() const {return canonicalType_;}
    // 获取结构体/联合体/枚举类型的名称
    std::string getName() const {
        if (decl_) {
            return decl_->getSymbol()->getName();
        }
        return "";
    }
    // 获取结构体/联合体/枚举类型的大小
    int getSize() const {
        if (decl_) {
            return decl_->getSymbol()->getType()->getSize();
        }
        return 0; // 如果没有声明，返回0
    }
    // 获取结构体/联合体/枚举类型的对齐方式
    int getAlign() const {
        if (decl_) {
            return decl_->getSymbol()->getType()->getAlign();
        }
        return 1; // 如果没有声明，返回1
    }
    // 获取结构体/联合体/枚举类型的字段数量
    int getFieldCount() const {
        if (decl_) {
            if (auto recordDecl = dynamic_cast<RecordDecl*>(decl_)) {
                return recordDecl->getFieldCount();
            }
        }
        return 0; // 如果没有声明或不是RecordDecl，返回0
    }
    // 获取结构体/联合体/枚举类型的字段列表
    std::vector<FieldDecl*> getFields() const {
        std::vector<FieldDecl*> fields; 
        if (decl_) {
            if (auto recordDecl = dynamic_cast<RecordDecl*>(decl_)) {
                fields = recordDecl->getFields();
            }
        }
        return fields; // 如果没有声明或不是RecordDecl，返回空列表
    }
    // 获取结构体/联合体/枚举类型的字段偏移量
    unsigned getFieldOffset(const std::string& fieldName) const {
        if (decl_) {
            if (auto recordDecl = dynamic_cast<RecordDecl*>(decl_)) {
                for (const auto& field : recordDecl->getFields()) {
                    if (field->getSymbol()->getName() == fieldName) {
                        return field->getOffset();
                    }
                }
            }
        }
        return 0; // 如果没有声明或字段不存在，返回0
    }
    // 获取结构体/联合体/枚举类型的字段类型
    QualType getFieldType(const std::string& fieldName) const {
        if (decl_) {
            if (auto recordDecl = dynamic_cast<RecordDecl*>(decl_)) {
                for (const auto& field : recordDecl->getFields()) {
                    if (field->getSymbol()->getName() == fieldName) {
                        return field->getQualType();
                    } 
                }
            }
        }
        return QualType(); // 如果没有声明或字段不存在，返回空类型
    }
    // 获取结构体/联合体/枚举类型的字段符号
    Symbol* getFieldSymbol(const std::string& fieldName) const {
        if (decl_) {
            if (auto recordDecl = dynamic_cast<RecordDecl*>(decl_)) {
                for (const auto& field : recordDecl->getFields()) {
                    if (field->getSymbol()->getName() == fieldName) {
                        return field->getSymbol();
                    }
                }
            }
        }
        return nullptr; // 如果没有声明或字段不存在，返回nullptr
    }
};

class EnumType : public TagType {
protected:
    EnumType(Decl *dc)
    : TagType(Type::ENUM, QualType(), dc) {}
public:
    static EnumType* NewObj(Decl *dc);
    static EnumType* NewObj(QualType can, Decl *dc) {
        return new EnumType(dc);
    }
    bool isEnumType() const {return kind_ == Type::ENUM;}
    bool isRecordType() const {return kind_ == Type::RECORD;}
    bool isTagType() const {return kind_ == Type::ENUM || kind_ == Type::RECORD;}
    bool isIncompleteType() const {return !process_;}
    bool isObjectType() const {return true;} // 枚举类型是对象类型
    bool isFunctionType() const {return false;} // 枚举类型不是函数类型
    bool isTypeName() const {return true;} // 枚举类型是类型名
    // 获取枚举类型的规范类型
    QualType getCanonicalType() const {return canonicalType_;}
    // 获取枚举类型的名称
    std::string getName() const {
        if (decl_) {
            return decl_->getSymbol()->getName();
        }
        return "";
    }
    // 获取枚举类型的大小
    int getSize() const {
        if (decl_) {
            return decl_->getSymbol()->getType()->getSize();
        }
        return 0; // 如果没有声明，返回0
    }
    // 获取枚举类型的对齐方式
    int getAlign() const {
        if (decl_) {
            return decl_->getSymbol()->getType()->getAlign();
        }
        return 1; // 如果没有声明，返回1
    }
    // 获取枚举类型的枚举常量数量
    int getEnumConstantCount() const {
        if (decl_) {
            if (auto enumDecl = dynamic_cast<EnumDecl*>(decl_)) {
                return enumDecl->getEnumConstantCount();
            }
        }
        return 0; // 如果没有声明或不是EnumDecl，返回0
    }
    // 获取枚举类型的枚举常量列表
    std::vector<EnumConstantDecl*> getEnumConstants() const {
        std::vector<EnumConstantDecl*> constants;
        if (decl_) {
            if (auto enumDecl = dynamic_cast<EnumDecl*>(decl_)) {
                constants = enumDecl->getEnumConstants();
            }
        }
        return constants; // 如果没有声明或不是EnumDecl，返回空列表
    }
    // 获取枚举类型的枚举常量值
    int getEnumConstantValue(const std::string& constantName) const {
        if (decl_) {
            if (auto enumDecl = dynamic_cast<EnumDecl*>(decl_)) {
                for (const auto& constant : enumDecl->getEnumConstants()) {
                    if (constant->getSymbol()->getName() == constantName) { 
                        return constant->getValue();
                    }
                }
            }
        }
        return 0; // 如果没有声明或常量不存在，返回0
    }
    // 获取枚举类型的枚举常量符号
    Symbol* getEnumConstantSymbol(const std::string& constantName) const {  
        if (decl_) {
            if (auto enumDecl = dynamic_cast<EnumDecl*>(decl_)) {
                for (const auto& constant : enumDecl->getEnumConstants()) {
                    if (constant->getSymbol()->getName() == constantName) {
                        return constant->getSymbol();
                    }
                }
            }
        }
        return nullptr; // 如果没有声明或常量不存在，返回nullptr
    }  
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
    bool isTypedefType() const { return kind_ == Type::TYPEDEF; }
    bool isObjectType() const { return true; } // typedef类型是对象类型
    bool isFunctionType() const { return false; } // typedef类型不是函数类型
    bool isTypeName() const { return true; } // typedef类型是类型名
    // 获取typedef类型的规范类型
    QualType getCanonicalType() const { return canonicalType_; }
    // 获取typedef类型的名称
    std::string getName() const {
        if (decl_) {
            return decl_->getSymbol()->getName();
        }
        return "";
    }
    // 获取typedef类型的大小
    int getSize() const {
        if (decl_) {
            return decl_->getSymbol()->getType()->getSize();
        }
        return 0; // 如果没有声明，返回0
    }
    // 获取typedef类型的对齐方式
    int getAlign() const {
        if (decl_) {
            return decl_->getSymbol()->getType()->getAlign();
        }
        return 1; // 如果没有声明，返回1
    }
    // 获取typedef类型的基础类型
    QualType getBaseType() const {
        if (decl_) {
            return decl_->getQualType();
        }
        return QualType(); // 如果没有声明，返回空类型
    }
    // 获取typedef类型的符号
    Symbol* getSymbol() const {
        if (decl_) {
            return decl_->getSymbol();
        }
        return nullptr; // 如果没有声明，返回nullptr
    }
    // 获取typedef类型的声明
    Decl* getDecl() const {
        return decl_;
    }
    // 设置typedef类型的声明
    void setDecl(Decl* decl) {
        decl_ = dynamic_cast<TypedefDecl*>(decl);
        assert(decl_ != nullptr && "TypedefType must have a TypedefDecl");
    }
    // 获取typedef类型的限定符
    unsigned getQualifiers() const {
        return canonicalType_.getQual();
    }
    // 判断是否是const限定符
    bool isConst() const {
        return canonicalType_.isConstQual();
    }
    // 判断是否是volatile限定符
    bool isVolatile() const {
        return canonicalType_.isVolatileQual();
    }
    // 判断是否是restrict限定符
    bool isRestrict() const {
        return canonicalType_.isRestrictQual();
    }
    // 判断是否是指针类型
    bool isPointerType() const {
        return canonicalType_.getPtr()->isPointerType();
    }
    // 判断是否是数组类型
    bool isArrayType() const {
        return canonicalType_.getPtr()->isArrayType();
    }
    // 判断是否是函数类型
    bool isFunctionType() const {
        return canonicalType_.getPtr()->isFunctionType();
    }
    // 判断是否是结构体类型
    bool isStructType() const {
        return canonicalType_.getPtr()->isStructType();
    }
    // 判断是否是联合体类型
    bool isUnionType() const {
        return canonicalType_.getPtr()->isUnionType();
    }
    // 判断是否是枚举类型
    bool isEnumType() const {
        return canonicalType_.getPtr()->isEnumType();
    }
    // 判断是否是基本类型
    bool isBuiltinType() const {
        return canonicalType_.getPtr()->getKind() == Type::BUILTIN;
    }
    // 判断是否是复数类型
    bool isComplexType() const {
        return canonicalType_.getPtr()->getKind() == Type::FLOAT_COMPLEX ||
               canonicalType_.getPtr()->getKind() == Type::DOUBLE_COMPLEX ||
               canonicalType_.getPtr()->getKind() == Type::LONGDOUBLE_COMPLEX;
    }
    // 判断是否是void类型
    bool isVoidType() const {
        return canonicalType_.getPtr()->getKind() == Type::VOID;
    }
    // 判断是否是字符类型
    bool isCharType() const {
        return canonicalType_.getPtr()->getKind() == Type::BUILTIN &&
               canonicalType_.getPtr()->getWidth() == 8;
    }
    // 判断是否是整数类型
    bool isIntegerType() const {
        return canonicalType_.getPtr()->getKind() == Type::BUILTIN &&
               (canonicalType_.getPtr()->getWidth() == 8 ||
                canonicalType_.getPtr()->getWidth() == 16 ||
                canonicalType_.getPtr()->getWidth() == 32 ||
                canonicalType_.getPtr()->getWidth() == 64);
    }
    // 判断是否是浮点类型
    bool isFloatingType() const {
        return canonicalType_.getPtr()->getKind() == Type::BUILTIN &&
               (canonicalType_.getPtr()->getWidth() == 32 ||
                canonicalType_.getPtr()->getWidth() == 64 ||
                canonicalType_.getPtr()->getWidth() == 128);
    }
    // 判断是否是布尔类型
    bool isBoolType() const {   
        return canonicalType_.getPtr()->getKind() == Type::BUILTIN &&
               canonicalType_.getPtr()->getWidth() == 8;
    }
    // 判断是否是长整型
    bool isLongType() const {
        return canonicalType_.getPtr()->getKind() == Type::BUILTIN &&
               (canonicalType_.getPtr()->getWidth() == 32 ||
                canonicalType_.getPtr()->getWidth() == 64);
    }
    // 判断是否是短整型
    bool isShortType() const {
        return canonicalType_.getPtr()->getKind() == Type::BUILTIN &&
               (canonicalType_.getPtr()->getWidth() == 8 ||
                canonicalType_.getPtr()->getWidth() == 16);
    }
    // 判断是否是长双精度类型
    bool isLongDoubleType() const {
        return canonicalType_.getPtr()->getKind() == Type::BUILTIN &&
               canonicalType_.getPtr()->getWidth() == 128;
    }
    // 判断是否是双精度类型
    bool isDoubleType() const {
        return canonicalType_.getPtr()->getKind() == Type::BUILTIN &&
               canonicalType_.getPtr()->getWidth() == 64;
    }
    // 判断是否是单精度类型
    bool isFloatType() const {
        return canonicalType_.getPtr()->getKind() == Type::BUILTIN &&
               canonicalType_.getPtr()->getWidth() == 32;
    }
    // 判断是否是复数类型
    bool isComplexFloatType() const {
        return canonicalType_.getPtr()->getKind() == Type::FLOAT_COMPLEX;
    }
    bool isComplexDoubleType() const {
        return canonicalType_.getPtr()->getKind() == Type::DOUBLE_COMPLEX;
    }
    bool isComplexLongDoubleType() const {
        return canonicalType_.getPtr()->getKind() == Type::LONGDOUBLE_COMPLEX;
    }
    // 判断是否是复数类型
    bool isComplexType() const {
        return canonicalType_.getPtr()->getKind() == Type::FLOAT_COMPLEX ||
               canonicalType_.getPtr()->getKind() == Type::DOUBLE_COMPLEX ||
               canonicalType_.getPtr()->getKind() == Type::LONGDOUBLE_COMPLEX;
    }
};