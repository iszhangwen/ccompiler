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
#include "arena.h"

// 前向声明
class Value;
class User;

class Use : public ArenaNode<Use>
{
public:
    explicit Use(Value* val, User* user)
    : m_value(val), m_user(user){}

    bool operator==(const Use& other){
        return (this->m_user == other.m_user) && (this->m_value == other.m_value);
    }

private:
    Value* m_value;
    User* m_user;
};

class Value : public ArenaNode<Value>
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

    // use-def维护
    void addUse(Use use) {m_uses.push_back(use);}
    void removeUse(Use use) {m_uses.remove(use);}
    vid addUse(User* user) {addUse(Use(this, user))}
    void removeUse(User* user) {removeUse(Use(this, user));}
    void clearUse() {m_uses.clear();}
    std::list<Use> getUses() {return m_uses;}

private:
    std::string m_name;
    QualType m_type; // 类型
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

    Value* getOperand(unsigned index);
    void setOperand(unsigned index, Value* val);
    void addOperand(Value* val);

private:
    unsigned int m_operandNum;
    std::vector<Value*> m_operands;
};
