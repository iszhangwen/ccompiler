#include "decl.h"

EnumConstantDecl::EnumConstantDecl(Symbol* id, Expr* val)
: ValueDecl(NodeKind::NK_EnumConstantDecl, id, val->getType()), initExpr_(val) {}