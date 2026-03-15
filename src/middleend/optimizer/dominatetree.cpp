#include "dominatetree.h"

#include "module.h"
#include "function.h"
#include "block.h"

#include <algorithm>

/*
该pass计算出每个节点的支配前沿，用于mem2reg算法插入phi节点
计算过程：
（1）通过dfs计算后每个基本块的逆后序。
（2）迭代计算获取每个节点的直接支配节点 LCA算法
*/

bool DominateTree::runOnModule(Module* ptr)
{
    assert(ptr);
    for (auto func : ptr->getFunctions()) {
        if (func->getBasicBlocks().empty())
            continue;
        createIdom(func);   
        createIdomFront(func);
    }
    return true;
}

void DominateTree::getPostOrder(BasicBlock* bb, std::unordered_set<BasicBlock*>& isVisited)
{
    isVisited.insert(bb);
    for (auto& tmp : bb->getSuccessors()) {
        if (isVisited.count(tmp))
            continue;
        getPostOrder(tmp, isVisited);
    }
    m_bb2int[bb] = m_postOrder.size();
    m_postOrder.push_back(bb);
}

BasicBlock* DominateTree::intersect(BasicBlock* bb1, BasicBlock* bb2)
{
    while (bb1 != bb2) {
        // 如果bb1的支配深度小于bb2，则说明bb1原理根节点
        // 则使得bb1为他的祖先节点并进行再次比较
        // 获取：bb1与bb2在支配树上的最近公共祖先节点
        while (m_bb2int[bb1] < m_bb2int[bb2]) {
            bb1 = m_idoms[m_bb2int[bb1]];
        }
        while (m_bb2int[bb1] > m_bb2int[bb2]) {
            bb2 = m_idoms[m_bb2int[bb2]];
        }
    }
    return bb1;
}

void DominateTree::createIdom(Function* ptr)
{
    assert(ptr);
    // 清理资源
    m_postOrder.clear();
    m_bb2int.clear();
    m_idoms.clear();
    // 通过DFS计算每个节点的逆后序
    // 调整顺序，将起始的节点放到数组前面，方便迭代
    auto entry = ptr->getEntryBlock();
    std::unordered_set<BasicBlock*> isVisited;
    getPostOrder(entry, isVisited);
    std::reverse(m_postOrder.begin(), m_postOrder.end());

    // 定义支配数组,并且记录支配深度
    for (auto v : m_postOrder) {
        m_idoms.push_back(nullptr);
    }

    // 根节点的直接支配者为自身：特殊处理
    m_idoms[m_bb2int[entry]] = entry;

    // 通过迭代，计算所有节点的直接支配者
    bool isChanged = true;
    while (isChanged) {
        for (auto node : m_postOrder) {
            isChanged = false;
            // 边界值：entry的支配着已经计算出，不需要迭代
            if (node == entry)
                continue;
            // 1. 获取node节点上第一个有直接支配者的前驱子
            // 直接支配者计算方法是在支配树上计算所有前驱子的LCA
            BasicBlock* newIdom = nullptr; 
            for (auto preNode : node->getPredecessors()) {
                if (m_idoms[m_bb2int[preNode]] != nullptr) {
                    newIdom = preNode;
                }
            }
            // 2. 取得newIdom之后，通过LCA算法计算所有前驱子最近公共的支配点。
            if (newIdom) {
                for (auto preNode : node->getPredecessors()) {
                    newIdom = intersect(newIdom, preNode);
                }
            }
            // 3. 取得idom后，检测到idom有变化，则说明本轮迭代结束后，需要进行下一次迭代
            if (m_idoms[m_bb2int[node]] != newIdom) {
                m_idoms[m_bb2int[node]] = newIdom;
                isChanged = true;
            }
        }
    }
    // 计算出每个基本块的idom后，将idom设置到基本块中
    for (auto node : m_postOrder) {
        node->setIdom(m_idoms[m_bb2int[node]]);
    }
}

bool DominateTree::isIdom(BasicBlock* bb1, BasicBlock* bb2)
{
    // 检查bb1是否支配bb2
    if (bb1 == bb2)
        return true;
    auto runner = bb2;
    while (runner) {
        if (runner == bb1)
            return true;
        runner = m_idoms[m_bb2int[runner]];
    }
    return false;
}

void DominateTree::createIdomFront(Function* ptr)
{
    assert(ptr);
    // 清空资源
    for (auto& node : ptr->getBasicBlocks()){
        node->clearDomFrontier();
    }
    // 计算支配前沿
    for (auto& node : ptr->getBasicBlocks()){
        // 支配前沿的计算方法：
        // 对每个基本块

        // 1. 如果node的前驱子为1个或者0个，则node必定不是支配前沿
        if (node->getPredecessors().size() < 2)
            continue;

        // 2. 对node的每个前驱子，沿着支配树计算直到node的直接支配点为止
        // 路径上的所有节点都满足: 
        for (auto preNode : node->getPredecessors()) {
            auto runner = preNode;
            while (runner && node->getIdom() != runner) {
                runner->addDomFrontier(node);
                runner = runner->getIdom();
            }
        }
    }
    // 核心概念: Cytron SSA论文
}