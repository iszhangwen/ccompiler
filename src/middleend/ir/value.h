#pragma once

#include "usedef.h"


// 参考clang项目
// 右值对象
// 右值对象分为：标量，复杂类型，聚合类型三种
// 右值返回值
class RValue 
{
public:
    enum RVType {
        Scalar,
        Complex,
        Aggregate,
    };
    bool isScalar() const {return m_type == Scalar;}
    bool isComplex() const {return m_type == Complex;}
    bool isAggregate() const {return m_type == Aggregate;} 

    Value* getScalar() {return m_value1;}

private:
    RVType m_type;
    Value* m_value1;
    Value* m_value2;
};


// 左值对象: 简单对象和数组对象
// 左值返回地址
class LValue 
{
public:
    enum LVType {
        Simple,
        ArrayElt,
        BitField,
    };
    // 位域
    struct BitFiledData {
        unsigned start;
        unsigned size;
    };
    LValue(Value* addr)
    : m_type(Simple), m_value(addr), m_ArrayIdx(nullptr){}
    LValue(Value* addr, Value* idx)
    : m_type(ArrayElt), m_value(addr), m_ArrayIdx(idx){}

    bool isSimple() const {return m_type == Simple;}
    bool isArrayElt() const {return m_type == ArrayElt;}
    // simple lvalue
    Value* getAddress() const { assert(isSimple()); return m_value;}
    // vector elt lvalue
    Value* getArrayAddr() const { assert(isArrayElt()); return m_value;}
    Value* getArrayIdx() const { assert(isArrayElt()); return m_ArrayIdx;}

private:
    LVType m_type;
    Value* m_value;
    Value* m_ArrayIdx;
    BitFiledData m_bitFiledData;
};