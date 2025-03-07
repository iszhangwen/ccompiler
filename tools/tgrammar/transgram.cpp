#include "transgram.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <deque>
#include <unordered_set>

NodeKey CFGGrammarData::findNode(const std::string& name) {
    for (auto iter = cfg.begin(); iter != cfg.end(); ++iter) {
        if (iter->first->name == name) {
            return iter->first;
        }
    }
    return std::make_shared<Node>(Node::unknow, name);
}

NodeKey CFGGrammarData::InsertNode(const std::string& nodename, Node::NodeKind ntype) {
    auto tmp = findNode(nodename);
    if (tmp->kind != Node::unknow) {
        return tmp;
    }
    NodeKey node = std::make_shared<Node>(ntype, nodename);
    auto [key, val] = cfg.insert({node, std::vector<CFGProduct>{}});
    if (!val) {
        std::cout << "insert: " << nodename << " failed!\n";
    }
    return node;
}

void CFGGrammarData::InsertProduct(const std::string& nodename, const std::string& pcLine) {    
    // 插入key
    auto key = findNode(nodename);
    if (key->kind == Node::unknow) {
        key = InsertNode(nodename, Node::mid);
    } else if (key->kind == Node::end) {
        key->kind = Node::mid;
    }
    CFGProduct pc;
    std::istringstream iss(pcLine);
    std::string word;
    while (iss >> word) {
        auto val = findNode(word);
        if (val->kind == Node::unknow) {
            val = InsertNode(word, Node::end);
        }
        pc.push_back(word);
    }
    cfg[key].push_back(pc);
}

void CFGGrammarData::InsertProduct(const std::vector<std::string>& block) {
    if (block.size() < 2) {
        return;
    }
    std::string tmp = block.at(0);
    std::string nodename = tmp.substr(0, tmp.size() - 1);
    for (int i = 1; i < block.size(); i++) {
        InsertProduct(nodename, block.at(i));
    }
}

void CFGGrammarData::DumpProduct()
{
    for (auto [key, _] : cfg) {
        std::cout << key->name << std::endl;
    }
}

int CFGGrammarData::generateKeySequence(const std::string& name) {
    auto key = findNode(name);
    int index = key->sequence + 1;
    auto iter = cfg.find(key);

    while ((++iter) != cfg.end()) {
        if (iter->first->sequence <= index) {
            index++;
        }
    }
    return index;
}

bool CFGGrammarData::count(const std::string& name) {
    for (auto iter: cfg) {
        if (iter.first->name == name) {
            return true;
        }
    }
    return false;
}

/*
@ brief 建立语法map：要求任何产生式中的元素，都是对Node的引用。
*/
bool CFGGrammar::buildCfg(const std::string& filename)
{
    std::ifstream ifile(filename);
    try
    {
        if (!ifile.is_open()) {
            throw std::runtime_error("buildcfg open file failed.");
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    std::string line;
    std::vector<std::string> block;
    while (std::getline(ifile, line)) {
        if (!line.empty()) {
            block.push_back(line);
        }
        if (line.empty() || ifile.eof()) {
            if (block.empty()) {
                continue;
            }
            CFGTree.InsertProduct(block);
            block.clear();
        }
    }
    ifile.close();
    // 分配标记
    CFG2LL1();
    return true;
}

void CFGGrammar::dumpLL1(const std::string& start, std::stringstream& stream)
{
    int index = 1;
    int step = 1000;
    std::deque<std::string> de;
    std::unordered_set<std::string> stf;
    de.push_back(start);
    while (!de.empty()) {
        auto name = de.front();
        de.pop_front();
        if (!stf.count(name) && CFGTree.count(name)) {
            stf.insert(name);
            auto key = CFGTree.findNode(name);
            key->sequence = index;
            index += step;
            if (key->kind == Node::mid) {
                stream << name  << ":" << "\n";
                for (auto& pc: CFGTree[name]) {
                    for (auto word: pc) {
                        stream << " " << word << " ";
                        de.push_back(word);
                    }
                    stream << "\n";
                }
                stream << "\n";
            }
        }
    }
}

void CFGGrammar::dumpLL1(const std::string& start, const std::string& filename)
{
    std::ofstream ofile(filename);
    try
    {
        if (!ofile.is_open()) {
            throw std::runtime_error("buildcfg open file failed.");
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return;
    }
    
    std::stringstream ss;
    dumpLL1(start, ss);
    ofile << ss.str();
    ofile.close();
}

void CFGGrammar::dumpLL1(const std::string& start)
{
    std::stringstream ss;
    dumpLL1(start, ss);
    std::cout << ss.str();
}

bool CFGGrammar::CFG2LL1()
{
    eliminateIndirectLR();
    return true;
}

// 消除直接左递归
void CFGGrammar::eliminateDirectLR(const std::string& name)
{
    auto key = CFGTree.findNode(name);
    if (key->kind != Node::mid) {
        return;
    }
    // 含左递归产生式 A(pv1)
    std::vector<CFGProduct> pv1;
    pv1.push_back(std::deque<std::string>{"e"}); 
    // 不含左递归产生式 pv2
    std::vector<CFGProduct> pv2;
    // 插入A'终结符，形如 A' -> pv1A' | e
    int base =  CFGTree.generateKeySequence(name);
    std::string keyString = name + std::to_string(base - key->sequence);
    // 分隔产生式
    for (auto& de: CFGTree[name]) {
        if (de.front() == name) {
            de.pop_front();
            de.push_back(keyString);
            pv1.push_back(de);
        } else {
            de.push_back(keyString);
            pv2.push_back(de);
        }
    }
    // A -> pv2A'
    CFGTree[name] = pv2;
    // A' -> aA'|e
    if (pv1.size() > 1) {
        auto newKey = CFGTree.InsertNode(keyString, Node::mid);
        newKey->sequence = base;
        CFGTree[keyString] = pv1;
    }
}

// 消除间接左递归
bool CFGGrammar::eliminateIndirectLR()
{ 
    // 要求翻译开始的非终结符首元素不包括其他非终结符 
    // 因此排序时translation-unit排最后一个，并按照文法顺序排序
    // 广度搜索遍历树
    std::vector<std::string> vec;
    std::deque<std::string> de;
    int base = 1;
    int step = 1000;
    de.push_back(TARNS_UNIT);
    while (!de.empty()) {
        std::string cur = de.front();
        de.pop_front();
        auto key = CFGTree.findNode(cur);
        if (key->kind == Node::mid && key->sequence == 0) {
            CFGTree.findNode(cur)->sequence = (base++)*step;
            for (auto& pc : CFGTree[cur]) {
                for (auto& word: pc) {
                    de.push_back(word);
                }
            }
            vec.push_back(cur);
        }
    }
    std::reverse(vec.begin(), vec.end());

    // 消除间接左递归
    for (int i = 0; i < vec.size(); i++) {
        std::vector<CFGProduct> pcv(CFGTree[vec[i]]);
        std::cout << pcv.size() << "\n";
        for (int j = 0; j < i; j++) {
            // 对终结符i的每一个产生式，执行替换
            std::vector<CFGProduct> pcvj;
            for (auto iter = pcv.begin(); iter != pcv.end();) {
                if (iter->empty()) {
                    iter = pcv.erase(iter);
                    continue;
                }
                if (iter->front() == vec[j]) {
                    iter->pop_front();
                    for (auto& pcj : CFGTree[vec[j]]) {
                        CFGProduct pc(pcj);
                        pc.insert(pc.end(), iter->begin(), iter->end());
                        pcvj.push_back(pc);
                    }
                    iter = pcv.erase(iter);
                }  else {
                    ++iter;
                }
            }
            pcv.insert(pcv.end(), pcvj.begin(), pcvj.end());
        }
        CFGTree[vec[i]] = pcv;
        // 消除直接左递归
        eliminateDirectLR(vec[i]);
        std::cout << i << "/" << vec.size() << "  name: " << vec[i] << "\n";
    }
    return true;
}
