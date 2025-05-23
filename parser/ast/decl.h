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
#include "expr.h"
#include <scope.h>

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
class TranslationUnitDecl final : public Decl
{
public:
    static std::shared_ptr<TranslationUnitDecl> NewObj(std::vector<Decl> exDecl);
    virtual ~TranslationUnitDecl(){}
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    TranslationUnitDecl(std::vector<Decl> exDecl)
    : Decl(NodeKind::TranslationUnitDecl), exDeclList_(exDecl){}
    std::vector<Decl> exDeclList_;
};

// 所有具有名称的基类, 命名实体具备链接属性
class NamedDecl : public Decl
{
public:
    virtual ~NamedDecl(){}
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    NamedDecl(NodeKind nk, IdentifierInfo tk)
    : Decl(nk), name_(tk) {}
    NamedDecl()
    : Decl(NodeKind::Expr){}

private:
    IdentifierInfo name_;
};

// 标签声明:需要记录标签的名称和位置
/*
labeled-statement:
 identifier : statement
*/
class LabelDecl final : public NamedDecl 
{
public:
    static std::shared_ptr<LabelDecl> NewObj(IdentifierInfo tk, LabelStmt* ls);
    virtual ~LabelDecl(){}
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    LabelDecl(IdentifierInfo tk, LabelStmt* ls)
    : NamedDecl(NodeKind::LabelDecl, tk), label_(ls) {}

private:
    LabelStmt* label_;
};

// 带有值类型的声明，具备类型说明符: 比如变量，函数，枚举常量都需要类型信息。
class ValueDecl : public NamedDecl 
{
public:
    static std::shared_ptr<ValueDecl> NewObj(Token tk, TypeSpec ts);
    virtual ~ValueDecl(){}
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    ValueDecl(Token tk, TypeSpec ts)
    :NamedDecl(){}

private:
    TypeSpec ty_;
};

// 带有声明说明符的声明，声明说明符包括：存储说明符，类型限定符，类型说明符，比如变量，函数，参数等，
class DeclaratorDecl : public ValueDecl 
{
public:
    static std::shared_ptr<DeclaratorDecl> NewObj(Token tk, TypeSpec ts, StorageClass ss);
    virtual ~DeclaratorDecl(){}
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    DeclaratorDecl(Token tk, TypeSpec ts, StorageClass ss)
    : ValueDecl(tk, ts), ss_(ss) {}

private:
    StorageClass ss_;
};


// 变量声明：需要包含存储类，作用域，初始化表达式等
class VarDecl : public DeclaratorDecl
{
public:
    static std::shared_ptr<VarDecl> NewObj(Token tk, TypeSpec ts, StorageClass ss, Expr* ex = nullptr);
    virtual ~VarDecl(){}
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    VarDecl(Token tk, TypeSpec ts, StorageClass ss, Expr* ex = nullptr)
    : DeclaratorDecl(tk, ts, ss), initExpr_(ex){}

private:
    Expr* initExpr_; // 初始化表达式
};

/// 函数参数声明，需要默认值
class ParmVarDecl final : public VarDecl 
{
public:
    static std::shared_ptr<ParmVarDecl> NewObj(Token tk, TypeSpec ts, StorageClass ss, Expr* ex = nullptr);
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    ParmVarDecl(Token tk, TypeSpec ts, StorageClass ss, Expr* ex = nullptr)
    : VarDecl(tk, ts, ss, ex){}

};

// 函数声明: 
//(6.9.1) function-definition:
//   declaration-specifiers declarator declaration-listopt compound-statement
class FunctionDecl : public DeclaratorDecl
{
public:
    static std::shared_ptr<FunctionDecl> NewObj(Token tk, TypeSpec ts, StorageClass ss, std::vector<ParmVarDecl*>& param, CompoundStmt* body = nullptr);
    virtual void accept(std::shared_ptr<Vistor> vt) {}

protected:
    FunctionDecl(Token tk, TypeSpec ts, StorageClass ss, std::vector<ParmVarDecl*>& param, CompoundStmt* body)
    : DeclaratorDecl(tk, ts, ss), parm_(param), body_(body){}

private:
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