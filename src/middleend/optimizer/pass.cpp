#include "pass.h"

#include "module.h"
#include "function.h"
#include "block.h"

void PassManager::run(Module* obj)
{
    if (!obj) return;
    for (auto pass : m_passVec) {
        if (!pass) continue;
        // 模块及pass
        if (pass->isModulePass()) {
            pass->runOnModule(obj);
        }
        // 函数级pass
        if (pass->isFunctionPass()) {
            for (auto func : obj->getFunctions()) {
                if  (!func) continue;
                pass->runOnFunction(func);
            }
        }
        // 基本块级pass
        if (pass->isBasicBlockPass()) {
            for (auto func : obj->getFunctions()) {
                if  (!func) continue;
                for (auto block : func->getBasicBlocks()) {
                    if  (!block) continue;
                    pass->runOnBasicBlock(block);
                }
            } 
        }
    }
}