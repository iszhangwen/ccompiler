#pragma once

#include "usedef.h"

class Module;
/*
 * @brief: 为什么会有GlobalValue
 * 全局变量的地址和函数地址链接完成后都是常量
 * GlobalValue 主要用于表示全局作用域中的命名实体，包括函数，全局变量等
 * GlobalValue通常涉及链接（Linkage）和可见性（Visibility）
*/
class GlobalValue : public User
{
public:
    // 链接属性
    enum LinkType
    {
        ExternType,
        InternalType,
    };
    // 可见性
    enum Visibility
    {
        DefaultVisibility = 0,
        HiddenVisibility,
    };

    // 属性设置
    void setLinkType(LinkType val) {m_linkType = val;}
    void setVisibility(Visibility val) {m_visibility = val;}
    void setAlignment(int val) {m_alignment = val;}
    LinkType getLinkType() const {return m_linkType;}
    Visibility getVisibility() const {return m_visibility;}
    int getAlignment() const {return m_alignment;}

    // @brief: 获取所属的module
    Module* getParent() {return m_parent;}
    // @brief: 设置所属的module
    void setParent(Module* parent) {m_parent = parent;}

    // 值判断
    bool isFunction();
    bool isGlobalVariable();
    explicit GlobalValue(QualType, const std::string& name, Module* parent = nullptr);

private:
    int m_alignment;
    LinkType m_linkType;
    Visibility m_visibility;
    Module* m_parent;
};
