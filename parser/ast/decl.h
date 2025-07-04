/*
    设计原则：声明可以区分为编译单元 TranslationUnitDecl，
    声明可以分为三种：函数， 变量，类型
    声明包含：声明说明符（存储说明符 类型限定说明符 类型说明符） + 声明符(标识符， 指针， 数组，函数等)
    类层次设计:
    Decl---> TranslationUnitDecl
        ---> NamedDecl  ---> LabelDecl
                        ---> ValueDecl  ---> DeclaratorDecl     ---> VarDecl        ---> ParmVarDecl                                     
                                                                ---> FunctionDecl
                                                                ---> FiledDecl
                                        ---> EnumConstantDecl
                                        ---> IndirectFieldDecl
                        ---> TypeDecl   ---> typedefNameDecl    ---> typedefDecl    ---> EnumDecl
                                                                                    ---> RecordDecl
        ---> FileScopeAsmDecl
        ---> TopLevelStmtDecl
        ---> BlockDecl
        ---> CapturedDecl
        ---> EmptyDecl
*/

#pragma once
#include "ast.h"
#include "type.h"
#include "../lexer/token.h"

class Expr;
class CompoundStmt;
class IdentifierInfo;

// 声明说明符
struct DeclSpec
{
    int /*StorageClass*/ sc_;
    int /*TypeSpecifier*/ ts_;
    int /*TypeQualifier*/ tq_;
    int /*FuncSpecifier*/ fs_;
};

// 声明符
class Declarator
{
public:

};

class Decl : public AstNode
{
public:
    virtual ~Decl(){};
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    Decl(NodeKind nk): AstNode(nk) {}
};


/*
(6.9) translation-unit:
        external-declaration
        translation-unit external-declaration
 (6.9) external-declaration:
        function-definition
        declaration
*/
// 文件作用域
class TranslationUnitDecl final : public Decl
{
private:
    std::vector<Decl*> decl_;
public:
    TranslationUnitDecl(const std::vector<Decl*>& exDecl)
    : Decl(NK_TranslationUnitDecl), decl_(exDecl){}

    TranslationUnitDecl()
    : Decl(NK_TranslationUnitDecl){}

    void addDecl(std::vector<Decl*>&);
};

// 所有具有名称的基类, 命名实体具备链接属性
class NamedDecl : public Decl
{
protected:
    NamedDecl(NodeKind nk, IdentifierInfo* id)
    : Decl(nk), name(id){}
    NamedDecl()
    : Decl(NK_Expr){}

private:
    IdentifierInfo* name;
};

// 标签声明:需要记录标签的名称和位置
/*
labeled-statement:
 identifier : statement
*/
class LabelDecl final : public NamedDecl
{
public:
    static LabelDecl* NewObj(IdentifierInfo* id);

protected:
    LabelDecl(IdentifierInfo* id)
    : NamedDecl(NK_LabelDecl, id){}
};

// 带有值类型的声明，具备类型说明符: 比如变量，函数，枚举常量都需要类型信息。
class ValueDecl : public NamedDecl 
{
public:
    static ValueDecl* NewObj(IdentifierInfo* id, QualType ty);

protected:
    ValueDecl(NodeKind nk, IdentifierInfo* id, QualType ty)
    :NamedDecl(nk, id), ty_(ty){}

    ValueDecl(IdentifierInfo* id, QualType ty)
    :NamedDecl(NK_ValueDecl, id), ty_(ty){}

private:
    QualType ty_;
};

// 带有声明说明符的声明，声明说明符包括：存储说明符，类型限定符，类型说明符，比如变量，函数，参数等，
class DeclaratorDecl : public ValueDecl 
{
public:
    static DeclaratorDecl* NewObj(IdentifierInfo* id, QualType ty);

protected:
    DeclaratorDecl(NodeKind nk, IdentifierInfo* id, QualType ty)
    : ValueDecl(nk, id, ty){}

    DeclaratorDecl(IdentifierInfo* id, QualType ty)
    : ValueDecl(NK_DeclaratorDecl, id, ty){}

private:
    int sc_;
};

// 变量声明：需要包含存储类，作用域，初始化表达式等
class VarDecl : public DeclaratorDecl
{
public:
    static VarDecl NewObj(IdentifierInfo* id, QualType ty, Expr* ex=nullptr);

protected:
    VarDecl(NodeKind nk, IdentifierInfo* id, QualType ty, Expr* ex=nullptr)
    : DeclaratorDecl(nk, id, ty), initExpr_(ex){}

    VarDecl(IdentifierInfo* id, QualType ty, Expr* ex=nullptr)
    : DeclaratorDecl(NK_ValueDecl, id, ty), initExpr_(ex){}

private:
    Expr* initExpr_; // 初始化表达式
};

/// 函数参数声明，需要默认值
class ParmVarDecl final : public VarDecl 
{
public:
    static ParmVarDecl* NewObj(IdentifierInfo* id, QualType ty, Expr* ex=nullptr);

protected:
    ParmVarDecl(IdentifierInfo* id, QualType ty, Expr* ex=nullptr)
    : VarDecl(id, ty, ex){}

};

// 函数声明: 
//(6.9.1) function-definition:
//   declaration-specifiers declarator declaration-listopt compound-statement
class FunctionDecl : public DeclaratorDecl
{
public:
    static FunctionDecl* NewObj(IdentifierInfo* id, QualType ty, std::vector<ParmVarDecl*>& param, CompoundStmt* body);

protected:
    FunctionDecl(IdentifierInfo* id, QualType ty, std::vector<ParmVarDecl*>& param, CompoundStmt* body)
    : DeclaratorDecl(id, ty), parm_(param), body_(body){}

private:
    int fs_;
    Type* ty_;
    std::vector<ParmVarDecl*> parm_;
    CompoundStmt* body_;
};

/// Represents a member of a struct/union/class.
class FieldDecl : public DeclaratorDecl
{

};

//
class EnumConstantDecl : public ValueDecl
{

};

//
class IndirectFieldDecl : public ValueDecl
{

};

/// Represents a declaration of a type.
class TypeDecl : public NamedDecl 
{

};

class TypedefNameDecl : public TypeDecl
{

};

class TypedefDecl : public TypedefNameDecl 
{

};

class TagDecl : public TypeDecl
{

};

class EnumDecl : public TagDecl 
{

};

class RecordDecl : public TagDecl 
{

};

//
class FileScopeAsmDecl : public Decl 
{

};

class TopLevelStmtDecl : public Decl
{

};

class BlockDecl : public Decl
{

};

class CapturedDecl final : public Decl
{

};

class EmptyDecl : public Decl 
{

};