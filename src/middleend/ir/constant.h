#pragma once

// 常量是典型的零定义，表现为指令中的立即数等结构
// 常量一般存储在全局常量表中，避免多次重复创建
// 为了表示简单，实现时每次创建常量


#include "usedef.h"

class Constant : public User
{
public:
    Constant(QualType ty, const std::string &name, unsigned numOps)
    :User(ty, name, numOps){}

    // 常量类型判断
    virtual bool isConstantInt() {return false;}
    virtual bool isConstantFloat() {return false;}
    virtual bool isConstantString() {return false;}
    virtual bool isConstantChar() {return false;}
    virtual bool isConstantZero() {return false;}
};

class ConstantInt : public User
{
public:
    static int getValue(std::make_shared<ConstantInt> ptr) { return ptr->m_value; }
    int getValue() { return m_value; }
    static std::make_shared<ConstantInt> get(int val, Module *m);
    static std::make_shared<ConstantInt> get(bool val, Module *m);
    bool isConstantInt() {return true;}
    
private:
    ConstantInt(QualType ty, int val)
    : Constant(ty, "", 0), m_value(val){}
    int m_value;
};

class ConstantFloat : public User
{
public:
    static double getValue(std::make_shared<ConstantFloat> ptr) { return ptr->m_value; }
    double getValue() { return m_value; }
    static std::make_shared<ConstantFloat> get(double val) {return std::make_shared<ConstantInt>(nullptr, val);}
    bool isConstantFloat() {return true;}

private:
    ConstantFloat(QualType ty, double val)
    : Constant(ty, "", 0), m_value(val){}
    double m_value;
};

class ConstantString : public User
{
public:
    static int getValue(std::make_shared<ConstantString> ptr) { return ptr->m_value; }
    std::string getValue() { return m_value; }
    static std::make_shared<ConstantString> get(std::string val) {return std::make_shared<ConstantString>(nullptr, val);}
    bool isConstantString() {return true;}

private:
    ConstantString(QualType ty, std::string val)
    : Constant(ty, "", 0), m_value(val){}
    std::string m_value;

};

class ConstantChar : public User
{
public:
    static int getValue(std::make_shared<ConstantChar> ptr) { return ptr->m_value; }
    char getValue() { return m_value; }
    static std::make_shared<ConstantChar> get(char val) {return std::make_shared<ConstantChar>(nullptr, val);}
    bool isConstantChar() {return true;}

private:
    ConstantChar(QualType ty, char val)
    : Constant(ty, "", 0), m_value(val){}
    char m_value;
};

class ConstantZero : public User
{
public:
    static std::make_shared<ConstantZero> get(QualType ty) {return std::make_shared<ConstantZero>(ty);}
    bool isConstantZero() {return true;}

private:
    ConstantZero(QualType ty)
    : Constant(ty, "", 0){}
};


