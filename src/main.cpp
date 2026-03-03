#include <iostream>
#include "scanner.h"
#include "parse.h"
#include "astvisitor.h"
#include "irbuilder.h"

int main(int argc, char **argv)
{
    if (argc != 2) 
    {
        fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
        return 1;
    }
    // 开始解析
    try {
        Parser parse(argv[1]);
        // 前端词法语法分析
        parse.parseTranslationUnit();
        CodegenASTVisitor cg;
        parse.accept(&cg);
        // 中间代码生成
        IRBuilder irbuilder;
        parse.accept(&irbuilder);
        // 执行优化
        irbuilder.dump();
        // 后端代码生成
    }
    catch(std::exception& e)
    {
        std::cerr << e.what() << '\n';
        exit(-1);
    }
    catch(...)
    {
        std::cerr << "catch unknow error!" << '\n';
        exit(-1);
    }

    return 0;
}