#include "analyzer.h"
#include "type.h"

// 根据传入的ts返回内建类型
std::shared_ptr<Type> SemaAnalyzer::onActTypeSpec(int ts)
{
    // 解析符号位
    bool isSigned = (ts & TypeSpecifier::SIGNED);
    // 解析长度
    bool isShort = (ts & TypeSpecifier::SHORT);
    bool isLong = (ts & TypeSpecifier::LONG);
    bool isLongLong = (ts & TypeSpecifier::LONGLONG);
    // 解析类型
    bool isVoid = (ts & TypeSpecifier::VOID);
    bool isChar = (ts & TypeSpecifier::CHAR);
    bool isInt = (ts & TypeSpecifier::INT);
    bool isFloat = (ts & TypeSpecifier::FLOAT);
    bool isDouble = (ts & TypeSpecifier::DOUBLE);
    bool isBool = (ts & TypeSpecifier::_BOOL);
    bool isComplex = (ts & TypeSpecifier::_COMPLEX);
    // 空类型
    if (ts & TypeSpecifier::VOID) {    
        return std::make_shared<VoidType>();
    }  
    return nullptr;
}
