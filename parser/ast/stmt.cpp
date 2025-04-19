#include "stmt.h"

std::shared_ptr<LabelStmt> LabelStmt::NewObj(SourceLocation loc, const std::string& name)
{
    return std::make_shared<LabelStmt>(loc, name);
}
