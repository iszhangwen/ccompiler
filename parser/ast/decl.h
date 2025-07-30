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
#include <token.h>

class Symbol;

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
    int storageClass_; // 存储类说明符
    int funcSpec_; // 函数说明符    
public:
    Declarator(){}
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
};

class Decl : public AstNode
{
public:
    virtual ~Decl(){};
    virtual void accept(ASTVisitor* vt) {}

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
    // 获取声明的数量
    size_t getDeclCount() const {return dcs_.size();}
};

// 所有具有名称的基类, 命名实体具备链接属性
class NamedDecl : public Decl
{
    Declarator declarator_;
protected:
    NamedDecl(NodeKind nk, Declarator id)
    : Decl(nk), declarator_(id){}
    Declarator getDeclarator() const {
        return declarator_;
    }
    void setDeclarator(const Declarator& id) {
        declarator_ = id;
    }
public:
    std::string getName() const {
        return getDeclarator().getName();
    }
    void setName(const std::string& name) {
        getDeclarator().setName(name);
    }
};

// 标签声明:需要记录标签的名称和位置
class LabelDecl final : public NamedDecl
{
protected:
    LabelDecl(Declarator id)
    : NamedDecl(NK_LabelDecl, id){}
public:
    static LabelDecl* NewObj(Declarator id);
};

// 带有值类型的声明，具备类型说明符: 比如变量，函数，枚举常量都需要类型信息。
class ValueDecl : public NamedDecl 
{
protected:
    ValueDecl(NodeKind nk, Declarator id)
    :NamedDecl(nk, id){}

    ValueDecl(Declarator id)
    :NamedDecl(NK_ValueDecl, id){}

public:
    static ValueDecl* NewObj(Declarator id);
    QualType getType() const {
        return getDeclarator().getType();
    }
    void setType(QualType qt) {
        getDeclarator().setType(qt);
    }
};

// 带有声明说明符的声明，声明说明符包括：存储说明符，类型限定符，类型说明符，比如变量，函数，参数等，
class DeclaratorDecl : public ValueDecl 
{
protected:
    DeclaratorDecl(NodeKind nk, Declarator id)
    : ValueDecl(nk, id){}

    DeclaratorDecl(Declarator id)
    : ValueDecl(NK_DeclaratorDecl, id) {}
public:
    static DeclaratorDecl* NewObj(Declarator id);
    int getStorageClass() const {
        return getDeclarator().getStorageClass();
    }
    void setStorageClass(int sc) {
        getDeclarator().setStorageClass(sc);
    }
};

// 变量声明：需要包含存储类，作用域，初始化表达式等
class VarDecl : public DeclaratorDecl
{
    Expr* initExpr_; // 初始化表达式
protected:
    VarDecl(NodeKind nk, Declarator id, Expr* ex=nullptr)
    : DeclaratorDecl(nk, id), initExpr_(ex){}

    VarDecl(Declarator id, Expr* ex=nullptr)
    : DeclaratorDecl(NK_ValueDecl, id), initExpr_(ex){}
public:
    static VarDecl* NewObj(Declarator id, Expr* ex=nullptr);
    Expr* getInitExpr() {return initExpr_;}
    void setInitExpr(Expr* ex) {initExpr_ = ex;}
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
    DeclGroup getConstants() {return constants_;}
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
