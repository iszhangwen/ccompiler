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
#include "token.h"

class Scope;
struct Declarator
{
    enum DeclaratorKind {
        DK_UNDEFINED,
        DK_ARRAY,
        DK_VAR,
        DK_PARAM,
        DK_FUNC,
        DK_ENUMCONSTANT,
        DK_FIELD,
        DK_INDIRECTFILED,
        DK_TYPEDEFNAME,
        DK_RECORD,
        DK_ENUM
    };
    // 标识符类型
    DeclaratorKind m_kind;
    // 标识符名称
    std::string m_name;
    // 声明的类型
    QualType m_type;
    // 存储说明符
    StorageClass m_storageClass;
    // 函数说明符  
    FuncSpecifier m_funcSpec;
    // 构造函数
    Declarator()
    : m_kind(DK_UNDEFINED), m_name(""), m_type(nullptr), m_storageClass(N_SCLASS), m_funcSpec(N_FSPEC) {}
    Declarator(const std::string& name, QualType type, StorageClass sc, FuncSpecifier fs)
    : m_kind(DK_UNDEFINED), m_name(name), m_type(type), m_storageClass(sc), m_funcSpec(fs) {}

    DeclaratorKind getKind() {return m_kind;}
    void setKind(DeclaratorKind dk) {m_kind = m_kind;}

    std::string getName() {return m_name;}
    void setName(std::string name) {m_name = name;}

    QualType getType() {return m_type;}
    void setType(QualType ty) {m_type = ty;}

    StorageClass getStorageClass() {return m_storageClass;}
    void setStorageClass(StorageClass sc) {m_storageClass = sc;}

    FuncSpecifier getFuncSpec() {return m_funcSpec;}
    void setFuncSpec(FuncSpecifier funcSpec) {m_funcSpec = funcSpec;}
};

// 翻译单元
class TranslationUnitDecl final : public Decl
{
public:
    using DeclGroup = std::vector<std::shared_ptr<Decl>>;
    TranslationUnitDecl()
    : Decl(NK_TranslationUnitDecl){}

    virtual void accept(ASTVisitor* vt) override;
    // insert new decl
    void addDecl(const DeclGroup& dc) {m_decls.insert(m_decls.end(), dc.begin(), dc.end());}
    void addDecl(Decl* dc) {m_decls.push_back(dc);}
    const DeclGroup& getDecls() const {return m_decls;}
    size_t size() const {return m_decls.size();}

private:
    DeclGroup m_decls;
};

// 所有具有名称的基类, 命名实体具备链接属性
class NamedDecl : public Decl
{
public:
    NamedDecl(NodeKind nk)
    : Decl(nk){}
    std::string getName() {return m_name;}
    void setName(const std::string& name) {m_name = name;}

    std::shared_ptr<Scope> getScope(){return m_scope;}
    void setScope(std::shared_ptr<Scope> sc) {m_scope = sc;}

private:
    std::shared_ptr<Scope> m_scope;
    std::string m_name;
};

// 标签声明:需要记录标签的名称和位置
class LabelDecl final : public NamedDecl
{
public:
    LabelDecl()
    : NamedDecl(NK_LabelDecl) {}
    virtual void accept(ASTVisitor* vt) override;
};

// 带有值类型的声明，具备类型说明符: 比如变量，函数，枚举常量都需要类型信息。
class ValueDecl : public NamedDecl 
{
public:
    ValueDecl(NodeKind nk)
    : NamedDecl(nk){}
    virtual void accept(ASTVisitor* vt) override;

    QualType getQualType() {return m_type;}
    void setQualType(QualType qt) {m_type = qt;}

private:
    QualType m_type;
};

// 带有声明说明符的声明，声明说明符包括：存储说明符，类型限定符，类型说明符，比如变量，函数，参数等，
class DeclaratorDecl : public ValueDecl 
{
public:
    DeclaratorDecl(NodeKind nk)
    : ValueDecl(nk){}
    virtual void accept(ASTVisitor* vt) override;

    StorageClass getStorageClass() {return m_storageClass;}
    void setStorageClass(StorageClass sc) {m_storageClass = sc;}

private:
    StorageClass m_storageClass;
};

// 变量声明：需要包含存储类，作用域，初始化表达式等
class VarDecl : public DeclaratorDecl
{
public:
    VarDecl(NodeKind nk)
    : DeclaratorDecl(nk){}
    virtual void accept(ASTVisitor* vt) override;

    std::shared_ptr<Expr> getInitExpr() {return m_initExpr;}
    void setInitExpr(std::shared_ptr<Expr> ex) {m_initExpr = ex;}

private:
    std::shared_ptr<Expr> m_initExpr; // 初始化表达式
};

/// 函数参数声明，需要默认值
class ParmVarDecl final : public VarDecl 
{
public:
    ParmVarDecl()
    : VarDecl(NK_ParmVarDecl) {}
    virtual void accept(ASTVisitor* vt) override;
};

class FunctionDecl : public DeclaratorDecl
{
public:
    using ParamDeclGroup = std::vector<std::shared_ptr<ParmVarDecl>>;
    FunctionDecl()
    : DeclaratorDecl(NK_FunctionDecl), m_isDefinition(false), m_funSpec(N_FSPEC), m_body(nullptr) {}
    virtual void accept(ASTVisitor* vt) override;

    ParamDeclGroup getParmVarDeclList() {return m_parmDeclVars;}
    void setParmVarDeclList(ParamDeclGroup& vars) {m_parmDeclVars = vars;}

    std::shared_ptr<CompoundStmt> getBody() {return m_body;}
    void setBody(std::shared_ptr<CompoundStmt> body) {m_body = body;}

    bool getIsDefinition() const {return m_isDefinition;}
    void setIsDefinition(bool flag) {m_isDefinition = flag;}

private:
    bool m_isDefinition;
    FuncSpecifier m_funSpec;
    ParamDeclGroup m_parmDeclVars;
    std::shared_ptr<CompoundStmt> m_body;
};

/// Represents a member of a struct/union
class FieldDecl : public ValueDecl
{
public:
    FieldDecl()
    : ValueDecl(NK_FieldDecl), m_parent(nullptr), m_offset(0) {}
    virtual void accept(ASTVisitor* vt) override;

    std::shared_ptr<RecordDecl> getParent() {return m_parent;}
    void setParent(std::shared_ptr<RecordDecl>& parent) {m_parent = parent;}

    unsigned getOffset() {return m_offset;}
    void setOffset(unsigned offset) {m_offset = offset;}

private:
    std::shared_ptr<RecordDecl> m_parent;  // 所属的结构体/联合体
    unsigned m_offset;     // 字段在内存中的偏移量
};

class EnumConstantDecl : public ValueDecl
{
public:
    EnumConstantDecl()
    : ValueDecl(NK_EnumConstantDecl), m_initExpr(nullptr) {}
    virtual void accept(ASTVisitor* vt) override;

    std::shared_ptr<Expr> getInitExpr() {return m_initExpr;}
    void setInitExpr(std::shared_ptr<Expr> ex) {m_initExpr = ex;}

private:
    std::shared_ptr<Expr> m_initExpr;
};

class TypedefDecl : public ValueDecl 
{
public:
    TypedefDecl()
    : ValueDecl(NK_TypedefDecl) {}
    virtual void accept(ASTVisitor* vt) override;
};

class TagDecl : public NamedDecl
{
public:
    TagDecl(NodeKind nk)
    : NamedDecl(nk), m_isDefinition(false) {}
    bool isDefinition() const { return m_isDefinition;}
    void setDefinition(bool flag) {m_isDefinition = flag;}

private:
    bool m_isDefinition;  // 是否是定义（而非前向声明）
};

class EnumDecl : public TagDecl 
{
public:
    using EnumConstantDeclGroup = std::vector<std::shared_ptr<EnumConstantDecl>>;
    EnumDecl()
    : TagDecl(NK_EnumDecl){}
    virtual void accept(ASTVisitor* vt) override;

    void addConstant(std::shared_ptr<EnumConstantDecl> constant) {m_members.push_back(constant); }
    EnumConstantDeclGroup getConstants() {return m_members;}
    void setConstants(EnumConstantDeclGroup& dcs) {m_members = dcs;}

private:
    EnumConstantDeclGroup m_members;  // 枚举常量列表
};

class RecordDecl : public TagDecl 
{
public:
    using FieldDeclGroup = std::vector<std::shared_ptr<FieldDecl>>;
    RecordDecl()
    : TagDecl(NK_RecordDecl){}
    virtual void accept(ASTVisitor* vt) override;

    void addField(std::shared_ptr<FieldDecl> field) {m_filedDecls.push_back(field);}
    FieldDeclGroup getFiledDecls() {return m_filedDecls;}
    void setFiledDecls(FieldDeclGroup& fields) {m_filedDecls = fields;}

    bool isUnion() const {return m_isUnion;}
    void setIsUnion(bool flag) {m_isUnion = flag;}

private:
    bool m_isUnion;      // 是否是 union
    FieldDeclGroup m_filedDecls; // 字段列表
};
