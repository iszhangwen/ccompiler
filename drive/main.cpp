#include <iostream>
#include "scanner.h"

int main()
{
    scanner sc("/home/user/codehub/ccompiler/test/test.c");
    Token tn = sc.nextToken();
    while (tn.getKind() != TokenKind::EOF_) {
        tn = sc.nextToken();
        //std::cout << tn.getLocation().filename << " " 
        std::cout<< "line: " << tn.getLocation().line << "   " 
        << "coloum: " << tn.getLocation().coloum << "   " 
        << "TokenKind: " << (int)tn.getKind() << "   " 
        << "Value:  " << tn.getValue() << "\n"; 
    }
    std::cout << "hello LoxCompiler" << std::endl;
    return 0;
}