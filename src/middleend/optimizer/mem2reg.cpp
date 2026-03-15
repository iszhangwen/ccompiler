#include "mem2reg.h"
#include <cassert>

#include "module.h"
#include "function.h"
#include "block.h"
#include "instruction.h"

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
}


/*
@brief:  筛选可提升的alloca：
（1）位于函数的entry block
（2）类型为非变长，非不透明类型
（3）对所有该地址的访问都是load/store, 无指针算法
（4）地址未逃逸(未传递给其他函数，未取地址用于别名)
*/
void Mem2Reg::getPromotableVars(Function* ptr)
{
    assert(ptr);
    auto entry = ptr->getEntryBlock();
    for (auto inst : entry->getInsts()) {
        if (auto ainst = dynamic_cast<AllocaInst*>(inst)) {
        }
    }
}

/*
@brief：判断是否是可提升的alloca
*/
bool Mem2Reg::isPromotableVar(AllocaInst* ptr)
{
    if (!ptr) return false;
    for (auto use : ptr->get)
}
