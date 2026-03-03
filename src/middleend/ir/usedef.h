/*
use-def:
IR表达是一个graph, 每个node是一个value, 每个edge是一个use
*/

#pragma once
#include <memory>
#include <vector>
#include <list>
#include <any>

#include "type.h"

// 前向声明
class Value;
class User;

class Use
{
public:
    explicit Use(Value* val, User* user)
    : m_value(val), m_user(user){}

    friend bool operator==(const Use& a, const Use& b);
private:
    Value* m_value;
    User* m_user;
};

bool operator==(const Use& a, const Use& b)
{
    return (a.m_user == b.m_user) && (a.m_value == b.m_value);
}

class Value
{
public:
    explicit Value(QualType ty, const std::string &name)
    : m_type(ty), m_name(name){}
    ~Value() = default;
    // 属性设置
    std::string getName() const {return m_name;}
    void setName(std::string name) {m_name = name;}
    QualType getType() const {return m_type;}
    void setType(QualType ty) {m_type = ty;}
    std::any getMetaData() const {return m_metaData;}
    void setMetaData(std::any val) {m_metaData = val;}
    // use-def维护
    void addUse(Use use) {m_uses.push_back(use);}
    void removeUse(Use use) {m_uses.remove(use);}
    void clearUse() {m_uses.clear();}

private:
    std::string m_name;
    QualType m_type; // 类型
    std::any m_metaData;  //元数据
    std::list<Use> m_uses; // 使用边
};

class User : public Value
{
public:
    User(QualType ty, const std::string &name, unsigned numOps)
    : Value(ty, name), m_operandNum(numOps){
        m_operands.resize(m_operandNum);
    }
    ~User() = default;
    unsigned int getNumOperands() const {return m_operandNum;}
    
protected:
    std::shared_ptr<Value> getOperand(unsigned int index) {
        assert(index < m_operands.size() && "getOperand out of range");
        return m_operands[index];
    }
    void setOperand(unsigned int index, std::shared_ptr<Value> val) {
        assert(index < m_operands.size() && "setOperand() out of range!");
        m_operands[index] = val;  
        val->addUse(Use(val.get(), this));
    }
    void addOperand(std::shared_ptr<Value> val) {
        m_operandNum++;
        m_operands.push_back(val); 
        val->addUse(Use(val.get(), this));
    }

private:
    unsigned int m_operandNum;
    std::vector<std::shared_ptr<Value>> m_operands;
};
