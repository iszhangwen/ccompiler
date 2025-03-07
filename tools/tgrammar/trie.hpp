#pragma once
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <deque>
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

    void dumpTrie()
    {
        std::deque<std::shared_ptr<node>> de;
        de.push_back(root);

        while (!de.empty()) {
            auto tmp = de.front();
            de.pop_front();
            std::string key = tmp->value;
            for (auto iter = tmp->child.begin(); iter != tmp->child.end(); ++iter)
            {
                //cur = insertNode(cur, *iter);
            }
        } 
    }
};