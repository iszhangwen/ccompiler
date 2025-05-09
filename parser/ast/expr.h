#pragma once
#include "stmt.h"

// 
class ExprStmt : public Stmt
{
public:
    virtual void accept(std::shared_ptr<Vistor> vt){}
    ExprStmt(){}
private:

};