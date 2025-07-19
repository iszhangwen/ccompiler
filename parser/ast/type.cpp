#include "type.h"

PointerType* PointerType::newObj(QualType qt)
{
    return new PointerType(qt);
}
