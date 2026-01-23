#pragma once
// CFG is divided into three levels: module function block
// The module contains entry functions and function collections. The transfer of functions is represented by a call
// The function contains multiple blocks: including entry blocks and exit blocks. The transfer of blocks is represented by control flow edges.
//The block contains multiple statements
// Control flow graph definition: G = (V E)

#include "ast.h"

#include <vector>
#include <unordered_map>

// basicBlock
class CFGBlock;
class CFGEdge
{
public:
    int m_edgeType;
    std::shared_ptr<CFGBlock> m_source;
    std::shared_ptr<CFGBlock> m_target;
};
class CFGBlock
{
public:
    using StmtGroup = std::vector<std::shared_ptr<Stmt>>;
    using EdgeGroup = std::vector<std::shared_ptr<CFGEdge>>;
    CFGBlock(int id):m_id(id){}

    // property
    int id() const {return m_id;}
    void setId(int id) {m_id = id;}
    StmtGroup getStmts() const {return m_stmts;}
    EdgeGroup getPredecessors() const {return m_predecessors;}
    EdgeGroup getSuccessors() const {return m_successors;}
    void addStmt(std::shared_ptr<Stmt> val) {m_stmts.push_back(val);}
    void addPredecessor(std::shared_ptr<CFGEdge> edge) {m_predecessors.push_back(edge);}
    void addSuccessor(std::shared_ptr<CFGEdge> edge) {m_successors.push_back(edge);}
public:
    int m_id;
    StmtGroup m_stmts;
    EdgeGroup m_predecessors;
    EdgeGroup m_successors;
};

// function
class CFGFunction;
class CallSite
{
public:
    std::shared_ptr<CFGBlock> m_callerBlock;
    std::shared_ptr<CFGFunction> m_callee;
    std::shared_ptr<CFGBlock> m_retBlock;
};
class CFGFunction
{
public:
    template<typename  T> using GroupPtr = std::vector<std::shared_ptr<T>>;
    CFGFunction(std::string name):m_name(name){}

    std::string name() const {return m_name;}
    void setName(const std::string& name) {m_name = name;}
    void addBlock(std::shared_ptr<CFGBlock> block) {
        if (m_blocks.count(block->id())) {
            return;
        }
        m_blocks[block->id()] = block;
        if (m_blocks.size() == 1) {
            m_entry = block;
        }
    }
    std::shared_ptr<CFGBlock> getEntry() {return m_entry;}
    GroupPtr<CFGBlock> getExits() {return m_exits;}

    GroupPtr<ParmVarDecl> getParams() {return m_params;}
    void setParams(GroupPtr<ParmVarDecl> params) {m_params = params;}

    std::shared_ptr<Type> getType() {return m_retType;}
    void setType(std::shared_ptr<Type> type) {m_retType = type;}

    void recordCallSite(std::shared_ptr<CallSite> call) {m_callSites.push_back(call);}

private:
    // base property
    std::string m_name;
    std::shared_ptr<CFGBlock> m_entry;
    GroupPtr<CFGBlock> m_exits;
    std::unordered_map<int, std::shared_ptr<CFGBlock>> m_blocks;

    // parameters and return types
    GroupPtr<ParmVarDecl> m_params;
    std::shared_ptr<Type> m_retType;

    // Call points between functions
    GroupPtr<CallSite> m_callSites;
};

// module
class CFGModule
{
public:
    CFGModule(){}
    void addFunction(std::shared_ptr<CFGFunction> func){
        if (m_functions.count(func->name())) {
            return;
        }
        m_functions[func->name()] = func;
    }

    std::shared_ptr<CFGBlock> getGlobalInitBlock() {return m_globalInitBlock;}
    void setGlobalInitBlock(std::shared_ptr<CFGBlock> block) {m_globalInitBlock = block;}

    std::shared_ptr<CFGFunction> getMainFunction() {return m_mainFunction;}
    void setMainFunction(std::shared_ptr<CFGFunction> func) {m_mainFunction = func;}

public:
    // Global variable initialization basic block
    std::shared_ptr<CFGBlock> m_globalInitBlock;
    // main function
    std::shared_ptr<CFGFunction> m_mainFunction;
    // function collection
    std::unordered_map<std::string, std::shared_ptr<CFGFunction>> m_functions;
};





