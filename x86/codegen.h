#pragma once
#include <iostream>
#include <ast/ast.h>

class CodeGen
{
public:
    static void gen(AstNode* node);
};