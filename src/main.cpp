#include <iostream>
#include "scanner.h"
#include "parse.h"
#include "builder/irbuilder.h"
#include "pass.h"
#include "dominatetree.h"
#include "mem2reg.h"
#include "codegen.h"
#include "module.h"

bool compile(const std::string& infile, const std::string& outfile)
{
    // 前端词法语法分析器
    Parser parse;
    parse.run(infile);

    // 中间代码生成
    auto astIR = parse.getAstCtx();
    IRBuilder irbuilder;
    irbuilder.run(astIR);

    // 打印ir
    auto ssaIR = irbuilder.getModule();
    ssaIR->toStringPrint();
    // 执行代码优化
    //auto ssaIR = irbuilder.getModule();
    //PassManager passes;
    //passes.run(ssaIR);

    // 发射代码生成汇编
    return false;
}

int main(int argc, char **argv)
{
    if (argc != 2) 
    {
        fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
        return 1;
    }

    try {
        compile(argv[1], "");
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
