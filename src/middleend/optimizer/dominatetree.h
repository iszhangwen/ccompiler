#pragma once

#include "pass.h"
#include <cassert>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class DominateTree : public ModulePass
{
public:
    std::string getName() const override final {return std::string("dominateTree");}
    bool runOnModule(Module* ptr); 

private:
    // 计算支配树
    void createIdom(Function* ptr); 
    // 辅助函数：计算dfs逆后序
    void getPostOrder(BasicBlock* bb, std::unordered_set<BasicBlock*>& isVisited);
    // 辅助函数：LCA算法实现
    BasicBlock* intersect(BasicBlock* bb1, BasicBlock* bb2);

    // 计算支配前沿
    // 核心算法：B 支配 Y 的某个前驱，但 B 不严格支配 Y
    void createIdomFront(Function* ptr); 
    // 辅助函数：检查bb1是否支配bb2
    bool isIdom(BasicBlock* bb1, BasicBlock* bb2);

    // 逆后序存储数组
    std::vector<BasicBlock*> m_postOrder;
    // 存储基本块的支配深度信息，用于比较LCA算法
    std::unordered_map<BasicBlock*, int> m_bb2int;
    // 存储直接只配者信息
    std::vector<BasicBlock*> m_idoms;
};