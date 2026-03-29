#pragma once

// 常量是典型的零定义，表现为指令中的立即数等结构
// 常量一般存储在全局常量表中，避免多次重复创建
// 为了表示简单，实现时每次创建常量

#include "usedef.h"

/*
 * @brief: 为什么常量要继承于User
 * 常量可能属于常量表达式，因此会使用其他值
*/

class Constant : public User
{
public:
    Constant(QualType ty, const std::string &name = "")
    :User(ty, name){}

    // @brief: Value重载当前值是常量
    bool isConstant() override final {return false;}
    // @brief: 返回当前值是否为0
    bool isConstantZero();

    // @brief: 获取空值
    static Constant* getNullValue(QualType);
    // @brief: 获取1值
    static Constant* getOneValue(QualType);
};

class ConstantBool: public Constant
{
public:
    explicit ConstantBool(QualType ty, bool val);
    // @brief: 获取常量值
    bool getValue() {return m_value;}
    // @brief: 获取布尔对象
    static ConstantBool* get(QualType, bool);

    // @brief: 打印出IR
    void toStringPrint();
private:
    bool m_value;
};

class ConstantInt : public Constant
{
public:
    explicit ConstantInt(QualType ty, int val);
    // @brief: 获取常量值
    int getValue() {return m_value;}
    // @brief: 静态函数获取常量对象
    static ConstantInt* get(QualType, int);
    
    // @brief: 打印出IR
    void toStringPrint();

private:
    int m_value;
};

class ConstantFloat : public Constant
{
public:
    explicit ConstantFloat(QualType ty, float val);
    // @brief: 获取常量值
    float getValue() {return m_value;}
    // @brief: 静态函数获取常量对象
    static ConstantFloat* get(QualType, float);

    // @brief: 打印出IR
    void toStringPrint();

private:
    float m_value;
};



