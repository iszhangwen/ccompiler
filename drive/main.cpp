#include <iostream>
#include "scanner.h"

int main(int argc, char **argv)
{
    if (argc != 2) {
    fprintf(stderr, "%s: invalid number of arguments\n", argv[0]);
    return 1;
    }

    // 分词
    scanner sc(argv[1]);
    TokenSequence seq = sc.tokenize();
    seq.dump();
    return 0;
    // 代码生成
    Token tk = seq.next();

    printf("  .globl main\n");
    printf("main:\n");
    printf("  mov $%d, %%rax\n", atoi((tk.value_).c_str()));
    tk = seq.next();
    while (!tk.isEOF())
    {
        if (tk.kind_ == TokenKind::Addition_)
        {
            tk = seq.next();
            printf(" add $%d, %%rax\n", atoi(tk.value_.c_str()));
            tk = seq.next();
        }
        else if (tk.kind_ == TokenKind::Subtraction_)
        {
            tk = seq.next();
            printf(" sub $%d, %%rax\n", atoi(tk.value_.c_str()));
            tk = seq.next();
        }

    }
    printf("  ret\n");
    return 0;
}