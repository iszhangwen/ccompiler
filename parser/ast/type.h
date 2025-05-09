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
#include "ast.h"

// 类型限定符
enum class QualType
{
    RESTRICT,
    CONST,
    VOLATILE
};

// 类型说明符
enum class TypeSpec
{
    CHAR,
    UCHAR,
    SHORT,
    USHORT,
    INT,
    UINT,
    LONG,
    ULONG,
    LONGLONG,
    ULONGLONG,
    ENUM,
    FLOAT,
    DOUBLE,
    LONGDOUBLE,
    POINTER,
    VOID,
    UNION,
    STRUCT,
    ARRAY,
    FUNCTION,
    COUNT_
};

// Type是所有类型的基类类型
class Type
{
public:

protected:

private:
    // 类型限定符
    QualType qual_;
};

class VoidType : public Type
{

};

class ArithmType : public Type
{

};

class DerivedType : public Type
{

};

class PointerType : public DerivedType
{

};

class ArrayType : public DerivedType
{

};

class FuncType : public DerivedType
{

};

class StructType : public Type
{

};

// 结构体或联合体成员类型
struct Filed
{
    int offset;
    std::string name;
    int bits; // 位域位数
    int pos;  // 位域的开始位置, 收到机器大小端的影响，主流都是小端: 从低位开始分配内存，大端:从高位开始分配内存
    Type ty;
};

// 结构体或联合类型
class RecordType : public Type
{
    // 自定义类型名
    std::string name;
    // 成员
    std::vector<Filed> fileds;
    // 是否是完整对象 C99类型分为三种
    /*
    对象类型：完全描述对象的类型，函数类型：描述函数的类型， 不完整类型:描述类型大师缺乏确定其大小的信息类型。
    */
   int complete;
};

// 枚举类型
class EnumType : public Type
{
    std::string name;
    int complete;
};

