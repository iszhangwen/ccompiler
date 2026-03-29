#pragma once

#include "usedef.h"

class Function;
class BasicBlock;

class Argument : public Value
{
public:
    explicit Argument(QualType ty, const std::string& name, unsigned index,
                      Function* fun, BasicBlock* parent = nullptr)
    : Value(ty, name), m_index(index), m_function(fun), m_parent(parent){}

    // @brief: 获取所属函数
    Function* getFunction() {return m_function;}
    // @brief: 设置所属函数
    void setFunction(Function* fun) {m_function = fun;}
    // @brief: 获取所在基本块
    BasicBlock* getParent() {return m_parent;}
    // @brief: 设置所在基本块
    void setParent(BasicBlock* val) {m_parent = val;}
    // @brief: 获取参数序号
    unsigned getIndex() const {return m_index;}
    // @brief: 设置参数序号
    void setIndex(unsigned index) {m_index = index;}

    // @brief: 打印出IR
    void toStringPrint() override;

private:
    BasicBlock* m_parent;
    Function* m_function;
    unsigned int m_index;
};
