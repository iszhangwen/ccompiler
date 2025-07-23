#include "type.h"

BuiltinType::BuiltinType(int ts)
{
    // 解析类别
    kind_ = (ts & TypeSpecifier::VOID) 
    | (ts & TypeSpecifier::_BOOL) 
    | (ts & TypeSpecifier::CHAR) 
    | (ts & TypeSpecifier::INT) 
    | (ts & TypeSpecifier::FLOAT) 
    | (ts & TypeSpecifier::DOUBLE)
    | (ts & TypeSpecifier::LONGDOUBLE);
    // 解析符号
    isSigned_ = true;
    if (ts & TypeSpecifier::UNSIGNED) {
        isSigned_ = false;
    }
    // 解析长度
    width_ = (ts & TypeSpecifier::SHORT)
    | (ts & TypeSpecifier::LONG)
    | (ts & TypeSpecifier::LONGLONG);
}