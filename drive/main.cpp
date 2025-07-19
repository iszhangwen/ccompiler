#include <iostream>
#include "scanner.h"
#include "parse.h"
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
        parse.parseTranslationUnit();
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