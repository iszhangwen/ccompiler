#include <iostream>
#include "scanner.h"
#include "parse.h"
#include "irbuilder.h"
#include "pass.h"
#include "codegen.h"

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
        auto astUnit = parse.getTranslationUnit();
        // 语义分析：类型检查        
        // 中间代码生成
        IRBuilder irbuilder;
        astUnit->accept(&irbuilder);
        auto ssaModule = irbuilder.getModule();
        // 执行优化
        PassManager passm;
        passm.run(ssaModule);
        // 后端代码生成
        CodegenAssembler codegen;
        ssaModule->accept(&codegen);

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