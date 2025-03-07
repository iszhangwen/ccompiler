#include "transgram.h"
#include <iostream>

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "usage: input transform grammar path.\n";
        exit(-1);
    } 

    CFGGrammar cfg;
    cfg.buildCfg("/home/user/codehub/ccompiler/tools/tgrammar/grammar.txt");
    std::cout << "-------------------" << std::endl;

    if (argc == 2) {
        cfg.dumpLL1("translation-unit");
    }
    else {
        cfg.dumpLL1("translation-unit", std::string(argv[2]));
    }
    return 0;
}