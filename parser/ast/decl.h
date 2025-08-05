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

class Scope;

struct Declarator
{
    // 标识符名称
    std::string name_; 
    // 声明的类型
    QualType type_; 
    // 存储类
    int storageClass_; 
    // 函数说明符  
    int funcSpec_;   
    // 构造函数
    Declarator(const std::string& name, QualType type, int sc, int fs)
    : name_(name), type_(type), storageClass_(sc), funcSpec_(fs) {}
};

class Decl : public AstNode
{
public:
    virtual ~Decl(){};
    virtual void accept(std::shared_ptr<ASTVisitor> vt) {}

protected:
    Decl(NodeKind nk): AstNode(nk) {}
};

// 翻译单元
class TranslationUnitDecl final : public Decl
{
private:
    DeclGroup decls_;

public:
    TranslationUnitDecl(const DeclGroup& dc)
    : Decl(NK_TranslationUnitDecl), decls_(dc){}

    virtual void accept(ASTVisitor* vt) override;
    void addDecl(const DeclGroup& dc) {decls_.insert(decls_.end(), dc.begin(), dc.end());}
    void addDecl(Decl* dc) {decls_.push_back(dc);}
    const DeclGroup& getDecls() const {return decls_;}
    void setDecls(const DeclGroup& dc) {decls_ = dc;}
    size_t size() const {return decls_.size();}
};

// 所有具有名称的基类, 命名实体具备链接属性
class NamedDecl : public Decl
{
private:
    Scope* scope_;
    std::string name_;
public:
    NamedDecl(NodeKind nk, const std::string& name, Scope* sco)
    : Decl(nk), name_(name), scope_(sco) {}
    std::string getName() {return name_;}
    void setName(const std::string& name) {name_ = name;}

    Scope* getScope(){return scope_;}
    void setScope(Scope* sc) {scope_ = sc;}
};

// 标签声明:需要记录标签的名称和位置
class LabelDecl final : public NamedDecl
{
public:
    LabelDecl(const std::string& name, Scope* sco)
    : NamedDecl(NK_LabelDecl, name, sco) {}
    virtual void accept(ASTVisitor* vt) override;
};

// 带有值类型的声明，具备类型说明符: 比如变量，函数，枚举常量都需要类型信息。
class ValueDecl : public NamedDecl 
{
private:
    QualType ty_;
public:
    ValueDecl(NodeKind nk, const std::string& name, Scope* sco, QualType ty)
    : NamedDecl(nk, name, sco), ty_(ty) {}
    virtual void accept(ASTVisitor* vt) override;

    QualType getQualType() {return ty_;}
    void setQualType(QualType qt) {ty_ = qt;}
};

// 带有声明说明符的声明，声明说明符包括：存储说明符，类型限定符，类型说明符，比如变量，函数，参数等，
class DeclaratorDecl : public ValueDecl 
{
private:
    StorageClass sc_;
public:
    DeclaratorDecl(NodeKind nk, const std::string& name, Scope* sco, QualType ty, StorageClass sc)
    : ValueDecl(nk, name, sco, ty), sc_(sc) {}
    virtual void accept(ASTVisitor* vt) override;

    StorageClass getStorageClass() {return sc_;}
    void setStorageClass(StorageClass sc) {sc_ = sc;}
};

// 变量声明：需要包含存储类，作用域，初始化表达式等
class VarDecl : public DeclaratorDecl
{
private:
    Expr* initExpr_; // 初始化表达式
public:
    VarDecl(NodeKind nk, const std::string& name, Scope* sco, QualType ty, StorageClass sc, Expr* ex=nullptr)
    : DeclaratorDecl(nk, name, sco, ty, sc), initExpr_(ex) {}
    VarDecl(const std::string& name, Scope* sco, QualType ty, StorageClass sc, Expr* ex=nullptr)
    : DeclaratorDecl(NK_VarDecl, name, sco, ty, sc), initExpr_(ex) {}
    virtual void accept(ASTVisitor* vt) override;

    Expr* getInitExpr() {return initExpr_;}
    void setInitExpr(Expr* ex) {initExpr_ = ex;}
};

/// 函数参数声明，需要默认值
class ParmVarDecl final : public VarDecl 
{
public:
    ParmVarDecl(const std::string& name, Scope* sco, QualType ty, StorageClass sc, Expr* ex=nullptr)
    : VarDecl(NK_ParmVarDecl, name, sco, ty, sc, ex) {}
    virtual void accept(ASTVisitor* vt) override;
};

// 函数声明: 
class FunctionDecl : public DeclaratorDecl
{
private:
    FuncSpecifier fs_;
    std::vector<ParmVarDecl*> parmVarList_;
    CompoundStmt* body_;
public:
    FunctionDecl(const std::string& name, Scope* sco, QualType ty, StorageClass sc, FuncSpecifier fs, std::vector<ParmVarDecl*> param, CompoundStmt* body)
    : DeclaratorDecl(NK_FunctionDecl, name, sco, ty, sc), fs_(fs), parmVarList_(param), body_(body) {}
    virtual void accept(ASTVisitor* vt) override;

    std::vector<ParmVarDecl*> getParmVarDeclList() {return parmVarList_;}
    void setParmVarDeclList(std::vector<ParmVarDecl*>& vars) {parmVarList_ = vars;}

    CompoundStmt* getBody() {return body_;}
    void setBody(CompoundStmt* body) {body_ = body;}
};

/// Represents a member of a struct/union
class FieldDecl : public ValueDecl
{
private:
    RecordDecl* parent_;  // 所属的结构体/联合体
    unsigned offset_;     // 字段在内存中的偏移量

public:
    FieldDecl(const std::string& name, Scope* sco, QualType ty, RecordDecl* parent, unsigned offset)
    : ValueDecl(NK_FieldDecl, name, sco, ty), parent_(parent), offset_(offset) {}
    virtual void accept(ASTVisitor* vt) override;

    RecordDecl* getParent() { return reinterpret_cast<RecordDecl*>(parent_);  }
    unsigned getOffset() const { return offset_; }

    unsigned getOffset() {return offset_;}
    void setOffset(unsigned offset) {offset_ = offset;}
};

class EnumConstantDecl : public ValueDecl
{
private:
    Expr* initExpr_;

public:
    EnumConstantDecl(const std::string& name, Scope* sco, QualType ty, Expr* val)
    : ValueDecl(NK_EnumConstantDecl, name, sco, ty), initExpr_(val) {}
    virtual void accept(ASTVisitor* vt) override;

    Expr* getInitExpr() {return initExpr_;}
    void setInitExpr(Expr* ex) {initExpr_ = ex;}
};

class TypedefDecl : public ValueDecl 
{
public:
    TypedefDecl(const std::string& name, Scope* sco, QualType ty)
    : ValueDecl(NK_TypedefDecl, name, sco, ty) {}
    virtual void accept(ASTVisitor* vt) override;
};

class TagDecl : public NamedDecl
{
private:
    bool isDefinition_;  // 是否是定义（而非前向声明）

public:
    TagDecl(NodeKind nk, const std::string& name, Scope* sco,bool isDefinition)
    : NamedDecl(nk, name, sco), isDefinition_(isDefinition) {}
    bool isDefinition() const { return isDefinition_;}
    void setDefinition(bool flag) {isDefinition_ = flag;}
};

class EnumDecl : public TagDecl 
{
private:
    std::vector<EnumConstantDecl*> members_;  // 枚举常量列表

public:
    EnumDecl(const std::string& name, Scope* sco, bool isDefinition, const std::vector<EnumConstantDecl*>& members)
    : TagDecl(NK_EnumDecl, name, sco, isDefinition), members_(members) {}
    virtual void accept(ASTVisitor* vt) override;

    void addConstant(EnumConstantDecl* constant) {members_.push_back(constant); }
    std::vector<EnumConstantDecl*> getConstants() {return members_;}
    void setConstants(std::vector<EnumConstantDecl*>& dcs) {members_ = dcs;}
};

class RecordDecl : public TagDecl 
{
private:
    bool isUnion_;      // 是否是 union
    std::vector<FieldDecl*> filedDecls_; // 字段列表

public:
    RecordDecl(const std::string& name, Scope* sco, bool isDefinition, bool isUnion, std::vector<FieldDecl*> fields)
    : TagDecl(NK_RecordDecl, name, sco, isDefinition), isUnion_(isUnion), filedDecls_(fields) {}
    virtual void accept(ASTVisitor* vt) override;

    void addField(FieldDecl* field) {filedDecls_.push_back(field);}
    void setFiledDecls(std::vector<FieldDecl*>& fields) {filedDecls_ = fields;}
    std::vector<FieldDecl*> getFiledDecls() {return filedDecls_;}

    bool isUnion() const {return isUnion_;}
    void setIsUnion(bool flag) {isUnion_ = flag;}
};
