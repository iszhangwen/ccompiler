#pragma once
#include <ast.h>

class Type;
// 使用语法制导翻译方案，翻译过程即完成语义检查过程
// 语义分析主要有两个作用：AST构建，和语义分析
class SemaAnalyzer
{
private:
    SymbolTableContext* sys_; // 符号表上下文
public:
    SemaAnalyzer(SymbolTableContext* sys): sys_(sys) {}
    Type* onActTypeSpec(int ts);
};