#include <iostream>
#include <fstream>
#include <string>
#include "scanner.h"
#include "parse.h"
#include "builder/irbuilder.h"
#include "module.h"
#include "backend/backendpasses.h"
#include "backend/target/risc_v/riscvtargetmachine.h"

using namespace ccompiler;

bool compile(const std::string& infile, const std::string& outfile)
{
    // 前端词法语法分析器 + 语义分析
    Parser parse;
    parse.run(infile);

    // 检查语义错误
    auto diag = parse.getDiagnostic();
    if (diag && diag->hasErrors()) {
        return false;
    }

    // 中间代码生成
    auto astIR = parse.getAstCtx();
    IRBuilder irbuilder;
    irbuilder.run(astIR);

    // 获取SSA IR
    auto ssaIR = irbuilder.getModule();

    // 后端代码生成
    RISCVTargetMachine targetMachine;
    BackendPassManager backend;
    backend.setTargetMachine(&targetMachine);
    backend.run(ssaIR);

    // 输出汇编
    std::cout << backend.getAsmOutput();

    // 如果指定了输出文件，写入文件
    if (!outfile.empty()) {
        std::ofstream out(outfile);
        if (out.is_open()) {
            out << backend.getAsmOutput();
            out.close();
        }
    }

    return true;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
        return 1;
    }

    try {
        compile(argv[1], "tmp.s");
    }
    catch(std::exception& e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        exit(-1);
    }
    catch(...)
    {
        std::cerr << "catch unknow error!" << '\n';
        exit(-1);
    }

    return 0;
}
