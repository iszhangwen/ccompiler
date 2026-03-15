#pragma once

#include <string>
#include <vector>

#include "pass.h"

class AllocaInst;

class Mem2Reg : public ModulePass
{
public:
    std::string getName() const override final {return std::string("mem2reg");}
    bool runOnModule(Module* ptr); 

private:
    std::vector<AllocaInst*> m_allocas;

    // 获取可提升的AllocaInst
    void getPromotableVars(Function* ptr);
    // 判断是否是可提升局部变量
    static bool isPromotableVar(AllocaInst* ptr);
};