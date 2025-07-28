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
class RecordDecl;

class Declarator
{
public:
    enum DeclaratorKind {
        DK_Identifier, // 标识符
        DK_Pointer, // 指针
        DK_Array, // 数组
        DK_Function, // 函数
        DK_Paren, // 括号
        DK_Abstract // 抽象声明
    };
private:
    DeclaratorKind dk_;
    std::string name_; // 标识符名称
    QualType type_; // 声明的类型
    int storageClass_; // 存储类
    int funcSpec_; // 函数说明符    
public:
    Declarator(DeclaratorKind dk, const std::string& name, QualType type, int sc, int fs)
    : dk_(dk), name_(name), type_(type), storageClass_(sc), funcSpec_(fs) {}
    DeclaratorKind getKind() const { return dk_; }
    const std::string& getName() const { return name_; }
    QualType getType() const { return type_; }
    int getStorageClass() const { return storageClass_; }
    int getFuncSpec() const { return funcSpec_; }   
    void setType(QualType qt) { type_ = qt; }
    void setName(const std::string& name) { name_ = name; }
    void setStorageClass(int sc) { storageClass_ = sc; }    
    void setFuncSpec(int fs) { funcSpec_ = fs; }    
    static Declarator* NewObj(DeclaratorKind dk, const std::string& name, QualType type, int sc, int fs) {
        return new Declarator(dk, name, type, sc, fs);
    }
    static Declarator* NewObj(DeclaratorKind dk, const std::string& name, QualType type) {
        return new Declarator(dk, name, type, 0, 0);
    }
    static Declarator* NewObj(DeclaratorKind dk, const std::string& name) {
        return new Declarator(dk, name, QualType(), 0, 0);
    }
    static Declarator* NewObj(DeclaratorKind dk) {
        return new Declarator(dk, "", QualType(), 0, 0);
    }
};

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
protected:
    TranslationUnitDecl(const DeclGroup& dc)
    : Decl(NK_TranslationUnitDecl), dcs_(dc){}

    TranslationUnitDecl()
    : Decl(NK_TranslationUnitDecl){}

public:
    static TranslationUnitDecl* NewObj(const DeclGroup& dc);
    void addDecl(const DeclGroup& dc) {dcs_.insert(dcs_.end(), dc.begin(), dc.end());}
    void addDecl(Decl* dc) {dcs_.push_back(dc);}
    const DeclGroup& getDecls() const {return dcs_;}
    void setDecls(const DeclGroup& dc) {dcs_ = dc;}
    size_t size() const {return dcs_.size();}
    Decl* getDecl(size_t index) const {
        if (index < dcs_.size()) {
            return dcs_[index];
        }
        return nullptr; // 如果索引越界，返回nullptr
    }
    void removeDecl(size_t index) {
        if (index < dcs_.size()) {
            dcs_.erase(dcs_.begin() + index);
        }
    }
    // 获取声明的数量
    size_t getDeclCount() const {
        return dcs_.size();
    }
};

// 所有具有名称的基类, 命名实体具备链接属性
class NamedDecl : public Decl
{
    Symbol* sym_;
protected:
    NamedDecl(NodeKind nk, Symbol* id)
    : Decl(nk), sym_(id){}

public:
    Symbol* getSymbol() {return sym_;}
    void setSymbol(Symbol* id) {sym_ = id;}
};

// 标签声明:需要记录标签的名称和位置
class LabelDecl final : public NamedDecl
{
protected:
    LabelDecl(Symbol* id)
    : NamedDecl(NK_LabelDecl, id){}
public:
    static LabelDecl* NewObj(Symbol* id);
};

// 带有值类型的声明，具备类型说明符: 比如变量，函数，枚举常量都需要类型信息。
class ValueDecl : public NamedDecl 
{
    QualType ty_;
protected:
    ValueDecl(NodeKind nk, Symbol* id, QualType ty)
    :NamedDecl(nk, id), ty_(ty){}

    ValueDecl(Symbol* id, QualType ty)
    :NamedDecl(NK_ValueDecl, id), ty_(ty){}

public:
    static ValueDecl* NewObj(Symbol* id, QualType ty);
    QualType getQualType() {return ty_;}
    void setQualType(QualType qt) {ty_ = qt;}
};

// 带有声明说明符的声明，声明说明符包括：存储说明符，类型限定符，类型说明符，比如变量，函数，参数等，
class DeclaratorDecl : public ValueDecl 
{
    int sc_;
protected:
    DeclaratorDecl(NodeKind nk, Symbol* id, QualType ty, int sc)
    : ValueDecl(nk, id, ty), sc_(sc){}

    DeclaratorDecl(Symbol* id, QualType ty, int sc)
    : ValueDecl(NK_DeclaratorDecl, id, ty), sc_(sc){}
public:
    static DeclaratorDecl* NewObj(Symbol* id, QualType ty, int sc);
};

// 变量声明：需要包含存储类，作用域，初始化表达式等
class VarDecl : public DeclaratorDecl
{
    Expr* initExpr_; // 初始化表达式
protected:
    VarDecl(NodeKind nk, Symbol* id, QualType ty, int sc, Expr* ex=nullptr)
    : DeclaratorDecl(nk, id, ty, sc), initExpr_(ex){}

    VarDecl(Symbol* id, QualType ty, int sc, Expr* ex=nullptr)
    : DeclaratorDecl(NK_ValueDecl, id, ty, sc), initExpr_(ex){}
public:
    static VarDecl* NewObj(Symbol* id, QualType ty, int sc, Expr* ex=nullptr);
    Expr* getExpr() {return initExpr_;}
    void setExpr(Expr* ex) {initExpr_ = ex;}
};

/// 函数参数声明，需要默认值
class ParmVarDecl final : public VarDecl 
{
protected:
    ParmVarDecl(Symbol* id, QualType ty, int sc, Expr* ex=nullptr)
    : VarDecl(id, ty, sc, ex){}
public:
    static ParmVarDecl* NewObj(Symbol* id, QualType ty, int sc, Expr* ex=nullptr);
};

// 函数声明: 
class FunctionDecl : public DeclaratorDecl
{
    int fs_;
    DeclGroup param_;
    Stmt* body_;
protected:
    FunctionDecl(Symbol* id, QualType ty, int sc, DeclGroup param, Stmt* body)
    : DeclaratorDecl(id, ty, sc), param_(param), body_(body){}
public:
    static FunctionDecl* NewObj(Symbol* id, QualType ty, int sc, DeclGroup param, Stmt* body);
    Stmt* getBody() {return body_;}
    void setBody(Stmt* body) {body_ = body;}
};

/// Represents a member of a struct/union
class FieldDecl : public ValueDecl
{
    Decl* parent_;  // 所属的结构体/联合体
    unsigned offset_;     // 字段在内存中的偏移量

protected:
    FieldDecl(Symbol* id, QualType type, Decl* parent, unsigned offset)
    : ValueDecl(NodeKind::NK_FieldDecl, id, type), parent_(parent), offset_(offset) {}
public:
    static FieldDecl* NewObj(Symbol* id, QualType type, Decl* parent, unsigned offset);
    RecordDecl* getParent() { return reinterpret_cast<RecordDecl*>(parent_);  }
    unsigned getOffset() const { return offset_; }
};

class EnumConstantDecl : public ValueDecl
{
    Expr* initExpr_;
protected:
    EnumConstantDecl(Symbol* id,QualType qt, Expr* val)
    : ValueDecl(NodeKind::NK_EnumConstantDecl, id, qt), initExpr_(val) {}
public:
    static EnumConstantDecl* NewObj(Symbol* id, Expr* val);
    Expr* getInitExpr() const { return initExpr_;}
};

class TypedefDecl : public NamedDecl 
{
    QualType ty_;
protected:
    TypedefDecl(Symbol* id, QualType ty)
    : NamedDecl(NodeKind::NK_TypedefDecl, id), ty_(ty) {}
public:
    static TypedefDecl* NewObj(Symbol* id, QualType ty);
};

class TagDecl : public NamedDecl
{
    bool isDefinition_;  // 是否是定义（而非前向声明）
protected:
    TagDecl(NodeKind kind, Symbol* id, bool isDefinition)
        : NamedDecl(kind, id), isDefinition_(isDefinition) {}
public:
    bool isDefinition() const { return isDefinition_;}
};

class EnumDecl : public TagDecl 
{
    DeclGroup constants_;  // 枚举常量列表
protected:
    EnumDecl(Symbol* id, bool isDefinition)
        : TagDecl(NodeKind::NK_EnumDecl, id, isDefinition) {}

public:
    static EnumDecl* NewObj(Symbol* id, bool isDefinition);
    void addConstant(EnumConstantDecl* constant) {constants_.push_back(constant); }
    DeclGroup getConstants() {return constants_; }
};

class RecordDecl : public TagDecl 
{
    DeclGroup fields_;  // 字段列表
    bool isUnion_;      // 是否是 union
protected:
    RecordDecl(Symbol* id, bool isDefinition, bool isUnion)
    : TagDecl(NodeKind::NK_RecordDecl, id, isDefinition), isUnion_(isUnion) {}

public:
    static RecordDecl* NewObj(Symbol* id, bool isDefinition, bool isUnion);
    void addField(Decl* field) {fields_.push_back(field);}
    void addField(DeclGroup fields) { fields_.insert(fields_.end(), fields.begin(), fields.end());}
    DeclGroup getFields() {return fields_;}
    bool isUnion() const {return isUnion_; }
};
