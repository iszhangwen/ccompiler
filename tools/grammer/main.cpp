#include "transgram.h"
#include <iostream>

int main()
{
    CFGGrammar cfg;
    cfg.buildCfg("/home/user/codehub/ccompiler/tools/grammer/grammer.txt");
    std::cout << "--------hello world---------" << std::endl;
    cfg.dumpLL1();
    return 0;
}