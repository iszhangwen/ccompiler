#include <iostream>
#include "scanner.h"

int main()
{
    scanner sc("/home/user/codehub/ccompiler/test/test.c");
    Token tn = sc.next();
    while (tn.getKind() != TokenKind::EOF_) {
        tn = sc.next();
        //std::cout << tn.getLocation().filename << " " 
        std::cout<< "line: " << tn.getLocation().line << "   " 
        << "coloum: " << tn.getLocation().column << "   " 
        << "TokenKind: " << (int)tn.getKind() << "   " 
        << "Value:  " << tn.getValue() << "\n"; 
    }
    std::cout << "hello LoxCompiler" << std::endl;
    return 0;
}