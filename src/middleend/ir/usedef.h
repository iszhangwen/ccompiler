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

    bool operator==(const Use& other){
        return (this->m_user == other.m_user) && (this->m_value == other.m_value);
    }
    void setValue(Value* val) {m_value = val;}
    Value* getValue() {return m_value;}
    void setUser(User* user) {m_user = user;}
    User* getUser() {return m_user;}

private:
    // 被使用者
    Value* m_value;
    // 使用者
    User* m_user;
};

/*
 * @brief: Value是IR中所有可计算实体的抽象
*/
class Value 
{
public:
    explicit Value(QualType ty, const std::string& name = "")
    : m_type(ty), m_name(name){}
    virtual ~Value() = default;

    // 属性设置
    std::string getName() const {return m_name;}
    void setName(const std::string& name) {m_name = name;}
    QualType getType() const {return m_type;}
    void setType(QualType ty) {m_type = ty;}

    // use-def维护
    void addUse(Use use) {m_uses.push_back(use);}
    void removeUse(Use use) {m_uses.remove(use);}
    void addUse(User* user) {addUse(Use(this, user));}
    void removeUse(User* user) {removeUse(Use(this, user));}
    void clearUse() {m_uses.clear();}
    std::list<Use> getUses() {return m_uses;}

    // 常量判断
    virtual bool isConstant() {return false;}
    virtual bool isGlobal() {return false;}
    virtual bool isArgument() {return false;}
    bool isUndefValue(){return false;}

    // @brief:将当前所有使用该value的地方全部使用val进行替换，并断开use
    void replaceAllUseWith(Value* val);

    // 使用类型判断
    template<typename T>
    T* isa() {return dynamic_cast<T*>(this);}

    // @brief: 打印出IR
    virtual void toStringPrint(){}

private:
    std::string m_name;
    QualType m_type;
    std::list<Use> m_uses;
};

//@brief: user代表所有使用value的元素：包括指令，常量，全局变量等。
class User : public Value
{
public:
    User(QualType ty, const std::string &name = "")
    : Value(ty, name){}
    ~User() = default;

    // 操作数操作
    void resizeOperands(int);
    int getOperandsNumber() const;
    Value* getOperand(unsigned index);
    void setOperand(unsigned index, Value* val);
    void addOperand(Value* val);

private:
    std::vector<Value*> m_operands;
};
