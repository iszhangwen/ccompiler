#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

#include "pass.h"

class AllocaInst;
class Value;

class Mem2Reg : public ModulePass
{
public:
    std::string getName() const override final {return std::string("mem2reg");}
    bool runOnModule(Module* ptr); 

private:
    std::vector<AllocaInst*> m_allocas;

    // 变量版本栈
    std::unordered_map<Value*, std::stack<Value*>> m_varStack;

    // 获取可提升的AllocaInst
    static std::vector<AllocaInst*> getPromotableVars(Function* ptr);
    // 判断是否是可提升局部变量
    static bool isPromotableVar(AllocaInst* ptr);
    // 插入phi节点
    void insertPhiNode(std::vector<BasicBlock*>& defBlocks, AllocaInst* var);
    // phi节点重命名算法
    void rename(BasicBlock*);
    // 遍历use-def链，并替换
    void replaceAllUseWith(Value* oldVal, Value* newVal);
    // 删除alloca
    void removeAllocas();
};