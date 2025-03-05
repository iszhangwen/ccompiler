#pragma once
#include <string>
#include <vector>
#include <list>
#include <memory>
#include <sstream>
#include <iostream>
#include <unordered_map>

// define node
struct Node {
    enum NodeKind {
        end,
        mid
    };
    NodeKind kind;
    std::string name;
    Node(NodeKind k, const std::string& n)
    : kind(k), name(std::move(n)){}

    friend bool operator==(const Node& n1, const Node& n2) {
        return (n1.name == n2.name);
    }
};

class CFGGrammarData {
public:
    using CFGProduct = std::list<std::weak_ptr<Node>>;
    using NodeKey = std::shared_ptr<Node>; 

    struct NodeHash{
        size_t operator()(const NodeKey& n) const {
            return std::hash<std::string>()(n->name);
        }
    };
    struct NodeCmp {
        bool operator()(const NodeKey& n1, const NodeKey& n2) const {
            return n1->name == n2->name;
        }
    };

    NodeKey InsertNode(const std::string& nodename) {
        for (auto iter : cfg) {
            if (iter.first->name == nodename) {
                return iter.first;
            }
        }
        auto node = std::make_shared<Node>(Node::end, nodename);
        cfg.insert({node, std::vector<CFGProduct>()});
        return node;
    }

    void InsertProduct(const std::string& nodename, const std::string& pcLine) {
        CFGProduct pc;
        std::istringstream iss(pcLine);
        std::string word;
        while (iss >> word) {
           auto key = InsertNode(word);
           pc.push_back(std::weak_ptr<Node>(key));
        }
        cfg[InsertNode(nodename)].push_back(pc);
    }

    void InsertProduct(const std::vector<std::string>& block) {
        if (block.size() < 2) {
            return;
        }
        std::string nodename = block.at(0);
        for (int i = 1; i < block.size(); i++) {
            InsertProduct(nodename, block.at(i));
        }
    }

    void DumpProduct()
    {
        for (auto [key, _] : cfg) {
            std::cout << key->name << std::endl;
        }
    }

    bool count(const std::string& name) {
        for (auto iter: cfg) {
            if (iter.first->name == name) {
                return true;
            }
        }
        return false;
    }

    std::vector<CFGProduct>& operator[](const std::string& name) {
        for (auto iter: cfg) {
            if (iter.first->name == name) {
                return iter.second;
            }
        }
        throw std::runtime_error("find cfg failde.");
    }

private:
    std::unordered_map<NodeKey, std::vector<CFGProduct>, NodeHash, NodeCmp> cfg;
};

// define product

class CFGGrammar{
private:
    CFGGrammarData CFGTree;
    bool CFG2LL1();

public:
    bool buildCfg(const std::string& filename);
    void dumpCfg(const std::string& start, std::stringstream& stream);
    bool dumpLL1(const std::string& filename);
    bool dumpLL1();

};

