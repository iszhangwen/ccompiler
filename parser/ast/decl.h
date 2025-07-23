/*
    设计原则：声明可以区分为编译单元 TranslationUnitDecl，
    声明可以分为三种：函数， 变量，类型
    声明包含：声明说明符（存储说明符 类型限定说明符 类型说明符） + 声明符(标识符， 指针， 数组，函数等)
    类层次设计:
    Decl---> TranslationUnitDecl
        ---> NamedDecl  ---> LabelDecl
                        ---> ValueDecl  ---> DeclaratorDecl     ---> VarDecl        ---> ParmVarDecl                                     
                                                                ---> FunctionDecl
                                        ---> EnumConstantDecl
                                        ---> FiledDecl
                        ---> typedefDecl    ---> EnumDecl
                                            ---> RecordDecl
*/

#pragma once
#include "ast.h"
#include "type.h"
#include "../lexer/token.h"

class Expr;
class Stmt;
class Symbol;

class Decl : public AstNode
{
public:
    virtual ~Decl(){};
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    Decl(NodeKind nk): AstNode(nk) {}
};

// 翻译单元
class TranslationUnitDecl final : public Decl
{
    DeclGroup dcs_;
public:
    TranslationUnitDecl(const DeclGroup& dc)
    : Decl(NK_TranslationUnitDecl), dcs_(dc){}

    TranslationUnitDecl()
    : Decl(NK_TranslationUnitDecl){}

    void addDecl(const DeclGroup& dc) {
        dcs_.insert(dcs_.end(), dc.begin(), dc.end());
    }

};

// 所有具有名称的基类, 命名实体具备链接属性
class NamedDecl : public Decl
{
    Symbol* sym_;
public:
    NamedDecl(NodeKind nk, Symbol* id)
    : Decl(nk), sym_(id){}

    Symbol* getSymbol() {
        return sym_;
    }
    void setSymbol(Symbol* id) {
        sym_ = id;
    }
};

// 标签声明:需要记录标签的名称和位置
class LabelDecl final : public NamedDecl
{
public:
    LabelDecl(Symbol* id)
    : NamedDecl(NK_LabelDecl, id){}
};

// 带有值类型的声明，具备类型说明符: 比如变量，函数，枚举常量都需要类型信息。
class ValueDecl : public NamedDecl 
{
    QualType ty_;
public:
    ValueDecl(NodeKind nk, Symbol* id, QualType ty)
    :NamedDecl(nk, id), ty_(ty){}

    ValueDecl(Symbol* id, QualType ty)
    :NamedDecl(NK_ValueDecl, id), ty_(ty){}

};

// 带有声明说明符的声明，声明说明符包括：存储说明符，类型限定符，类型说明符，比如变量，函数，参数等，
class DeclaratorDecl : public ValueDecl 
{
    int sc_;
public:
    DeclaratorDecl(NodeKind nk, Symbol* id, QualType ty, int sc)
    : ValueDecl(nk, id, ty), sc_(sc){}

    DeclaratorDecl(Symbol* id, QualType ty, int sc)
    : ValueDecl(NK_DeclaratorDecl, id, ty), sc_(sc){}

};

// 变量声明：需要包含存储类，作用域，初始化表达式等
class VarDecl : public DeclaratorDecl
{
    Expr* initExpr_; // 初始化表达式
public:
    VarDecl(NodeKind nk, Symbol* id, QualType ty, int sc, Expr* ex=nullptr)
    : DeclaratorDecl(nk, id, ty, sc), initExpr_(ex){}

    VarDecl(Symbol* id, QualType ty, int sc, Expr* ex=nullptr)
    : DeclaratorDecl(NK_ValueDecl, id, ty, sc), initExpr_(ex){}

};

/// 函数参数声明，需要默认值
class ParmVarDecl final : public VarDecl 
{
public:
    ParmVarDecl(Symbol* id, QualType ty, int sc, Expr* ex=nullptr)
    : VarDecl(id, ty, sc, ex){}

};

// 函数声明: 
class FunctionDecl : public DeclaratorDecl
{
    int fs_;
    DeclGroup param_;
    Stmt* body_;
public:
    FunctionDecl(Symbol* id, QualType ty, int sc, DeclGroup param, Stmt* body)
    : DeclaratorDecl(id, ty, sc), param_(param), body_(body){}
    
    Stmt* getBody() {return body_;}
    void setBody(Stmt* body) {body_ = body;}
};

/// Represents a member of a struct/union
class FieldDecl : public ValueDecl
{
    RecordDecl* parent_;  // 所属的结构体/联合体
    unsigned offset_;     // 字段在内存中的偏移量
    bool isAnonymous_;    // 是否是匿名字段（如 union 内嵌的匿名 struct）

public:
    FieldDecl(Symbol* id, QualType type, RecordDecl* parent, unsigned offset, bool isAnonymous)
    : ValueDecl(NodeKind::NK_FieldDecl, id, type), parent_(parent), offset_(offset), isAnonymous_(isAnonymous) {}

    RecordDecl* getParent() const { 
        return parent_; 
    }
    unsigned getOffset() const { 
        return offset_; 
    }
    bool isAnonymous() const { 
        return isAnonymous_; 
    }
};

class EnumConstantDecl : public ValueDecl
{
    Expr* initExpr_;
public:
    EnumConstantDecl(Symbol* id, Expr* val);
    Expr* getInitExpr() const { 
        return initExpr_; 
    }
};

class TypedefDecl : public NamedDecl 
{
    QualType ty_;
public:
    TypedefDecl(Symbol* id, QualType ty)
    : NamedDecl(NodeKind::NK_TypedefDecl, id), ty_(ty) {}
};

class TagDecl : public NamedDecl
{
    bool isDefinition_;  // 是否是定义（而非前向声明）
public:
    TagDecl(NodeKind kind, Symbol* id, bool isDefinition)
        : NamedDecl(kind, id), isDefinition_(isDefinition) {}
    bool isDefinition() const { 
        return isDefinition_; 
    }
};

class EnumDecl : public TagDecl 
{
    DeclGroup constants_;  // 枚举常量列表
public:
    EnumDecl(Symbol* id, bool isDefinition)
        : TagDecl(NodeKind::NK_EnumDecl, id, isDefinition) {}
    void addConstant(EnumConstantDecl* constant) { 
        constants_.push_back(constant); 
    }
    DeclGroup getConstants() { 
        return constants_; 
    }
};

class RecordDecl : public TagDecl 
{
    DeclGroup fields_;  // 字段列表
    bool isUnion_;                    // 是否是 union
public:
    RecordDecl(Symbol* id, bool isDefinition, bool isUnion)
    : TagDecl(NodeKind::NK_RecordDecl, id, isDefinition), isUnion_(isUnion) {}
    void addField(Decl* field) { 
        fields_.push_back(field); 
    }
    void addField(DeclGroup fields) { 
        fields_.insert(fields_.end(), fields.begin(), fields.end());
    }
    DeclGroup getFields() { 
        return fields_; 
    }
    bool isUnion() const {
        return isUnion_; 
    }
};
