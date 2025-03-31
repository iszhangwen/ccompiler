#pragma once
#include <string>
#include <vector>
#include <deque>
#include <memory>
#include <sstream>
#include <iostream>
#include <map>

const std::string TARNS_UNIT = "translation-unit";
// define node
struct Node {
    enum NodeKind {
        end,
        mid,
        unknow
    };
    int sequence;
    NodeKind kind;
    std::string name;

    Node(NodeKind k, const std::string& n, int seq)
    : kind(k), name(std::move(n)), sequence(seq){}

    Node(NodeKind k, const std::string& n)
    : Node(k, n, 0){}

    friend bool operator==(const Node& n1, const Node& n2) {
        return (n1.name == n2.name);
    }
};

using CFGProduct = std::deque<std::string>;
using NodeKey = std::shared_ptr<Node>; 
class CFGGrammar;

class CFGGrammarData {
public:
    friend CFGGrammar;
    struct NodeCmp {
        // 降序排序
        bool operator()(const NodeKey& n1, const NodeKey& n2) const {
            return n1->name > n2->name;
        }
    };

    std::vector<CFGProduct>& operator[](const std::string& name) {
        for (auto& iter: cfg) {
            if (iter.first->name == name) {
                return iter.second;
            }
        }
        throw std::runtime_error("find cfg failde.");
    }

    void DumpProduct();
    bool count(const std::string& name);

    int generateKeySequence(const std::string& name);
    NodeKey findNode(const std::string& name);
    NodeKey InsertNode(const std::string& nodename, Node::NodeKind ntype);
    void InsertProduct(const std::string& nodename, const std::string& pcLine);
    void InsertProduct(const std::vector<std::string>& block);

private:
    std::map<NodeKey, std::vector<CFGProduct>, NodeCmp> cfg;
};

class CFGGrammar{
private:
    CFGGrammarData CFGTree;
    // 消除直接左递归
    void eliminateDirectLR(const std::string& name);
    // 消除间接左递归
    bool eliminateIndirectLR();
    // 消除回溯
    void eliminateBacktrace();
    // 文法规约
    bool CFG2LL1();
    // 求取非终结符号的select集合
    void getSelectCollection();

public:
    bool buildCfg(const std::string& filename);

    void dumpLL1(const std::string& start);
    void dumpLL1(const std::string& start, const std::string& filename);
    void dumpLL1(const std::string& start, std::stringstream& stream);
    void dumpCfg() {CFGTree.DumpProduct();}

};

