#include "constant.h"
#include "arena.h"

#include <unordered_map>
#include <iostream>

bool Constant::isConstantZero()
{
    if (auto obj = dynamic_cast<ConstantBool*>(this)) {
        return obj->getValue() == false;
    } else if (auto obj = dynamic_cast<ConstantInt*>(this)) {
        return obj->getValue() == 0;
    } else if (auto obj = dynamic_cast<ConstantFloat*>(this)) {
        return obj->getValue() == 0;
    }
    return false;
}

Constant* Constant::getNullValue(QualType ty)
{
    if (ty->isBoolType()) {
        return Arena::make<ConstantBool>(ty, false);
    } else if (ty->isIntegerType()) {
        return Arena::make<ConstantInt>(ty, 0);
    } else if (ty->isRealFloatingType()) {
        return Arena::make<ConstantFloat>(ty, 0);
    }
    return nullptr;
}

Constant* Constant::getOneValue(QualType ty)
{
    if (ty->isBoolType()) {
        return Arena::make<ConstantBool>(ty, true);
    } else if (ty->isIntegerType()) {
        return Arena::make<ConstantInt>(ty, 1);
    } else if (ty->isRealFloatingType()) {
        return Arena::make<ConstantFloat>(ty, 1);
    }
    return nullptr;
}

ConstantBool::ConstantBool(QualType ty, bool val)
    : Constant(ty, std::to_string(val))
{
    m_value = val;
}

// @brief: 打印出IR
void ConstantBool::toStringPrint()
{
    std::cout << (m_value ? "true" : "false") << "\n";
}

// @brief: 获取布尔对象
ConstantBool* ConstantBool::get(QualType ty, bool val)
{
    static std::unordered_map<int, ConstantBool*> objMap;
    if (objMap.count(val)) {
        return objMap[val];
    }
    objMap[val] = Arena::make<ConstantBool>(ty, val);
    return objMap[val];
}

ConstantInt::ConstantInt(QualType ty, int val)
    : Constant(ty, std::to_string(val))
{
    m_value = val;
}

// @brief: 打印出IR
void ConstantInt::toStringPrint()
{
    std::cout << m_value << "\n";
}


ConstantInt* ConstantInt::get(QualType ty, int val)
{
    static std::unordered_map<int, ConstantInt*> objMap;
    if (objMap.count(val)) {
        return objMap[val];
    }
    objMap[val] = Arena::make<ConstantInt>(ty, val);
    return objMap[val];
}

ConstantFloat::ConstantFloat(QualType ty, float val)
    : Constant(ty, std::to_string(val))
{
    m_value = val;
}

// @brief: 打印出IR
void ConstantFloat::toStringPrint()
{
    std::cout << m_value << "\n";
}

ConstantFloat* ConstantFloat::get(QualType ty, float val)
{
    static std::unordered_map<int, ConstantFloat*> objMap;
    if (objMap.count(val)) {
        return objMap[val];
    }
    objMap[val] = Arena::make<ConstantFloat>(ty, val);
    return objMap[val];
}
