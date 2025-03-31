#pragma once
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <map>
/*
    为了左公因子消除算法，实现前缀树
*/

class TrieGrammar {
private:
    struct node
    {
        std::string value;
        std::list<std::shared_ptr<node>> child;
        explicit node(const std::string& val):value(val) {}
    };
    std::shared_ptr<node> root;
    std::shared_ptr<node> insertNode(std::shared_ptr<node> pr, const std::string& name) {
        for (auto child : pr->child) {
            if (child->value == name) {
                return child;
            }
        }
        pr->child.push_back(std::make_shared<node>(name));
        return pr->child.back();
    }

public:
    // 创建前缀树时，为了解决消除回溯产生的多个相同产生式不同非终结符问题，建立key表
    void buildTrie(const std::string& key, const std::vector<std::deque<std::string>>& val) {
        root = std::make_shared<node>(key);
        for (int i = 0; i < val.size(); i++) {
            std::shared_ptr<node> cur = root;
            for (auto iter = val[i].begin(); iter != val[i].end(); ++iter)
            {
                cur = insertNode(cur, *iter);
            }
            // 每个产生式末尾必须标记leaf节点，代表路径的存在产生式
            cur->child.push_back(std::make_shared<node>("leaf"));
        }
    }

    void dumpTrie(std::map<std::string, std::vector<std::deque<std::string>>>& trie)
    {
        int index = 0;
        std::deque<std::shared_ptr<node>> de;
        de.push_back(root); 
        while (!de.empty()) {
            auto tmp = de.front();
            de.pop_front();
            std::string key = tmp->value;
            std::vector<std::deque<std::string>> val;
            for (auto iter = tmp->child.begin(); iter != tmp->child.end(); ++iter)
            {
                std::deque<std::string> pc;
                auto tmpNode = *iter;
                // 遍历每一个每一个子节点。直到出现分叉或无子节点
                // 当前节点是终结节点，直接插入e产生式并返回

                // 当前节点非叶节点(子孩子大于0)， 则区分分叉及不分叉两种情况
                while (tmpNode->child.size() > 0) {
                    if (tmpNode->child.size() == 1) {
                        if (tmpNode->value == "leaf") {
                            if (pc.empty()) {
                                pc.push_back("e");
                            }
                            break;
                        } else {
                            pc.push_back(tmpNode->value);
                            tmpNode = tmpNode->child.front();
                        }
                    } else {
                        pc.push_back(tmpNode->value);
                        std::string tmpKey = key + "_" + std::to_string(index++);
                        pc.push_back(tmpKey);
                        tmpNode->value = tmpKey;
                        de.push_back(tmpNode);
                        break;
                    }
                }
                if (!pc.empty()) {
                    val.push_back(pc);
                }
            }
            trie.insert({key, val});
        } 
    }
};