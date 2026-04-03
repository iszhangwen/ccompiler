#pragma once

#include <string>
#include <vector>
#include <memory>

class Module;
class Function;
class BasicBlock;

class Pass 
{
public:
    Pass(){}
    virtual ~Pass() = default;
    virtual std::string getName() const = 0;

    // @brief: 判断是否会更改IR ir分为analysis型， transform类型
    virtual bool isAnalysis() const {return false;}

    // @brief: 判断pass的执行级别
    virtual bool isModulePass() const = 0;
    virtual bool isFunctionPass() const = 0;
    virtual bool isBasicBlockPass() const = 0;

    // @brief: IR执行
    virtual bool runOnModule(Module* ptr)  = 0;
    virtual bool runOnFunction(Function* ptr)  = 0;
    virtual bool runOnBasicBlock(BasicBlock* ptr) = 0;

};

// 模块级pass
class ModulePass : public Pass
{
public:
    bool isModulePass() const override final {return true;}
    bool isFunctionPass() const override final {return false;}
    bool isBasicBlockPass() const override final {return false;}
    // @brief: pass执行
    bool runOnFunction(Function* ptr) override final {return false;}
    bool runOnBasicBlock(BasicBlock* ptr) override final {return false;}
};

// 函数级pass
class FunctionPass : public Pass
{
public:
    bool isModulePass() const override final {return false;}
    bool isFunctionPass() const override final {return true;}
    bool isBasicBlockPass() const override final {return false;}
    // @brief: pass执行
    bool runOnModule(Module* ptr)  override final {return false;}
    bool runOnBasicBlock(BasicBlock* ptr) override final {return false;}
};

// 基本块级pass
class BasicBlockPass : public Pass
{
public:
    bool isModulePass() const override final {return false;}
    bool isFunctionPass() const override final {return false;}
    bool isBasicBlockPass() const override final {return true;}
    // @brief: pass执行
    bool runOnModule(Module* ptr)  override final {return false;}
    bool runOnFunction(Function* ptr) override final {return false;}
};

class PassManager
{
public:
    void addPass(Pass* pass) {
        m_passVec.push_back(std::move(pass));
    }
    void run(Module*);

private:
    std::vector<Pass*> m_passVec;
};