#include "transgram.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <stack>
#include <unordered_set>

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
        if (line.empty()) {
            CFGTree.InsertProduct(block);
            continue;
        }
        block.push_back(line);
    }
    ifile.close();
    return true;
}

void CFGGrammar::dumpCfg(const std::string& start, std::stringstream& stream)
{
    std::unordered_set<std::string> stf;
    std::stack<std::string> st;
    st.push(start);
    while (!st.empty()) {
        auto name = st.top();
        st.pop();
        if (!stf.count(name) && CFGTree.count(name)) {
            stf.insert(name);
            stream << "\n" << name;
            for (auto& pc: CFGTree[name]) {
                for (auto word: pc) {
                    stream << (word.lock())->name << " ";
                    st.push((word.lock())->name);
                }
            }
        }
    }
}

bool CFGGrammar::dumpLL1(const std::string& filename)
{
    std::ofstream ofile(filename);
    if (!ofile.is_open()) {
        std::cout << "open file failed. dump ll(1)" << std::endl;
        return false;
    }
    dumpCfg("translation-unit:")
    // 使用广度优先搜索算法遍历CFG树
    return true;
}

bool CFGGrammar::dumpLL1()
{
    // 使用广度优先搜索算法遍历CFG树
    CFGTree.DumpProduct();

    return true;
}

bool CFGGrammar::CFG2LL1()
{
    return true;
}