#include "mem2reg.h"
#include <cassert>

#include "module.h"
#include "function.h"
#include "block.h"
#include "instruction.h"

#include <unordered_set>
#include <algorithm>

/*
内存模型与寄存器模型的对比
---------------------------------------------------------
维度	    内存模型	                         寄存器模型
变量存储	栈内存（alloca）	                 虚拟寄存器
读写方式	load/store	                        直接引用寄存器
生成阶段    前端生成的初始 IR	                 优化阶段转换后的 IR
优势	   生成简单、兼容性强	                 便于优化、贴近硬件
典型指令	alloca i32, load i32*, store i32	%x = add i32 %a, %b
-------------------------------------------------------------------
*/
/*
mem2reg的前提要求：
1. 为什么需要mem2reg:
why (1) 指令生成时使用了参数溢出的生成策略，即所有的参数否在栈上分配
    内存模型---寄存器模型的转化

1. Alloca提前： 指令生成时，alloca指令都会提前到函数的entry block。
why: (1) 简化内存管理模型：栈帧是一次性分配的(调用函数时压栈)，所以所有局部变量的内存都是函数调用开始前一次性分配的
    （2）便于后续优化： mem2reg需要知道 "alloca是否可以安全的提升为寄存器"，如果分散在不同块中分析起来很复杂
    （3）支持SSA构造：局部变量是寄存器的候选
*/

/*
mem2reg算法实现：基于Cytron 1991提出的高效SSA构造算法，主要分为四阶段：
    （1）筛选可提升alloca
    （2）Phi节点插入: 确定Phi节点位置 -- 支配边界
    （3）变量重命名：构建SSA值流 -- 支配树遍历+值栈
    （4）清理alloca, load/store等指令，简化Phi指令等。
how:
    (1) 筛选可提升alloca(promotable alloca): 主要是entry block块指令
    (2) 收集defblocks（所有store指令的位置）
    (3) 在PhibLOCK插入Phi节点
    (4) 遍历支配树，重命名(用值栈替换load/store)
    (5) 清理alloca, load, store
*/

bool Mem2Reg::runOnModule(Module* ptr)
{
    assert(ptr);
    for (auto func: ptr->getFunctions()) {
        // 1. 筛选可提升的局部变量
        auto promotableVar = getPromotableVars(func);
        // 2. Phi节点插入
        for (auto var : promotableVar) {
            // a. 收集每个变量的定义点和使用点
            std::vector<BasicBlock*> defBlocks;
            std::vector<BasicBlock*> useBlocks;
            for (auto user : var->getUses()) {
                if (auto store = dynamic_cast<StoreInst*>(user.getUser())) {
                    defBlocks.push_back(store->getParent());
                } else if (auto load = dynamic_cast<LoadInst*>(user.getUser())) {
                    useBlocks.push_back(load->getParent());
                } else {
                    return false;
                }
            }
            // b. 在支配前沿插入Phi节点
            insertPhiNode(defBlocks, var);
        }
        // 3. 变量重命名:值传播 ssa重命名(DFS + 栈)
        m_varStack.clear();
        rename(func->getEntryBlock());
    }
    // 4. 清理alloca/load/store指令
    removeAllocas();
    return false;
}


/*
@brief:  筛选可提升的alloca：
（1）位于函数的entry block
（2）类型为非变长，非不透明类型
（3）对所有该地址的访问都是load/store, 无指针算法
（4）地址未逃逸(未传递给其他函数，未取地址用于别名)
*/
std::vector<AllocaInst*> Mem2Reg::getPromotableVars(Function* ptr)
{
    assert(ptr);
    std::vector<AllocaInst*> res;
    auto entry = ptr->getEntryBlock();
    for (auto inst : entry->getInsts()) {
        if (auto allocaIns = dynamic_cast<AllocaInst*>(inst)) {
            if (isPromotableVar(allocaIns)) {
                res.push_back(allocaIns);
            }
        }
    }
    return res;
}

/*
@brief：判断是否是可提升的alloca
*/
bool Mem2Reg::isPromotableVar(AllocaInst* ptr)
{
    assert(ptr);
    for (auto ins : ptr->getUses()) {
        // promotable alloca指令允许load读取
        if (auto load = dynamic_cast<LoadInst*>(ins.getUser())) {
            continue;
        }
        // promotable alloca指令允许将值写入到地址，但是不能将alloca作为值写入到其他地址
        else if (auto store = dynamic_cast<StoreInst*>(ins.getUser())) {
            if (store->getValue() == ptr) {
                return false;
            }
            continue;
        }
        // promotable alloca指令只能被用于load/store指令
        else {
            return false;
        }
    }
    return true;
}

// PHI节点插入算法
// 在左右的支配前沿插入Phi节点
void Mem2Reg::insertPhiNode(std::vector<BasicBlock*>& defBlocks, AllocaInst* var)
{
    std::unordered_set<BasicBlock*> insertPhiInsts;
    while (!defBlocks.empty()) {
        auto parent = defBlocks.back();
        defBlocks.pop_back();
        for (auto domBlock : parent->getDomFrontier()) {
            if (!insertPhiInsts.count(domBlock)) {
                auto phi = Arena::make<PhiInst>(var->getType(), domBlock);
                phi->setValue(var); // 设置原始关联变量
                domBlock->addInst(phi); // 基本块插入指令
                insertPhiInsts.insert(domBlock); // 记录插入的phi节点
                // phi节点也是一处定义
                auto it = std::find(defBlocks.begin(), defBlocks.end(), domBlock);
                if (it == defBlocks.end()) {
                    defBlocks.push_back(domBlock);
                }
            }
        }
    }
}

// PHI 重命名 = 支配树 DFS + 变量版本栈 + 回溯
void Mem2Reg::rename(BasicBlock* block)
{
    std::vector<Value*> defVars;
    std::vector<Value*> deleteInsts;
    // 处理PHI节点
    for (auto inst : block->getInsts()) {
        if (auto phi = dynamic_cast<PhiInst*>(inst)) {
            auto var = phi->getValue();
            m_varStack[var].push(phi);
            defVars.push_back(var);
        }
    }
    
    // 处理非Phi节点，重命名
    for (auto inst : block->getInsts()) {
        if (auto load = dynamic_cast<LoadInst*>(inst)) {
            auto var = load->getAddr();
            // 替换load指令为当前值：寄存器值
            auto val = m_varStack[var].top();
            // 替换load为最新值
            replaceAllUseWith(inst, val);
            // 标记删除load指令
            deleteInsts.push_back(inst);
        }
        else if (auto store = dynamic_cast<StoreInst*>(inst)) {
            auto val = store->getValue();
            auto var = store->getAddr();
            // 替换load指令为当前值：寄存器值
             m_varStack[var].push(val);
            // 标记删除store指令
            deleteInsts.push_back(inst);
        }
    }

    // 为所有后继块的PHI节点填充incoming值
    for (auto suBlock : block->getSuccessors()) {
        for (auto inst : suBlock->getInsts()) {
            if (auto phi = dynamic_cast<PhiInst*>(inst)) {
                auto var = phi->getValue();
                auto val = m_varStack[var].top();
                phi->addIncoming(val, block);
            }
        }
    }

    // 递归处理支配树子节点
    for (auto child : block->getDomChildren()) {
        rename(child);
    }

    // 回溯：弹出本块定义的所有变量 保证不同路径的版本互不干扰
    for (auto& defVar : defVars) {
        m_varStack[defVar].pop();
    }

    // 删除本模块的load/store节点
    for (auto inst : deleteInsts) {
        if (auto ins = dynamic_cast<Instruction*>(inst))
            block->removeInst(ins);
    }
}

void Mem2Reg::replaceAllUseWith(Value* oldVal, Value* newVal)
{

}

void Mem2Reg::removeAllocas()
{
    
}
