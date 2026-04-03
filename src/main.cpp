#include <iostream>
#include "scanner.h"
#include "parse.h"
#include "builder/irbuilder.h"
#include "pass.h"
#include "dominatetree.h"
#include "mem2reg.h"
#include "module.h"

bool compile(const std::string& infile, const std::string& outfile)
{
    // 前端词法语法分析器
    Parser parse;
    parse.run(infile);

    /*
    * 中间代码生成,构建pipeline: 
    * 1. 生成ssa ir
    * 2. 执行优化pass
    */
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

    /*
    * 后端代码生成,构建pipeline: 
    * 1. 选择TargetMachine
    * 2. 指令选择pass
    * 3. 寄存器分配pass
    * 4. 指令调度pass
    * 5. 组装生成汇编pass
    */
    PassManager passes;
    passes.run(ssaIR);
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
