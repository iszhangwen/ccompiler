#pragma once

#include <string>
#include <vector>

class Scope; 
class Decl;
class TranslationUnitDecl;
class FunctionDecl;
class VarDecl;
class Type;
class QualType;

// 使用语法制导翻译方案，翻译过程即完成语义检查过程
// 语义分析主要有两个作用：AST构建，和语义分析
class sema
{
public:
    TranslationUnitDecl* onActTranslationUnit(Scope*, std::vector<Decl*>&);
    FunctionDecl* onActFunctionDecl();
    VarDecl* onActVarDecl();
    VarDecl* onActArrayDecl();

    Type* onActBuiltinType(int ts);
    Type* onActPointerType(QualType&);
};