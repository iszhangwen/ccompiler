#include "backendpasses.h"
#include "regAlloca/riscvasmemitter.h"
#include "module.h"
#include "function.h"

void BackendPassManager::run(Module* ssaModule)
{
    if (!m_targetMachine || !ssaModule) return;

    // 1. 指令选择
    m_instSelect.setTargetMachine(m_targetMachine);
    for (auto func : ssaModule->getFunctions()) {
        m_instSelect.runOnFunction(func);
    }

    // 2. 寄存器分配
    runRegAlloc();

    // 3. 发射汇编
    emitAsm();
}

void BackendPassManager::runRegAlloc()
{
    auto* machineModule = getMachineModule();
    if (!machineModule) return;

    LinearScanAllocPass regAlloc(m_targetMachine);

    for (auto* func : machineModule->getFunctions()) {
        regAlloc.run(func);
    }
}

void BackendPassManager::emitAsm()
{
    m_asmOutput.clear();
    auto* machineModule = getMachineModule();
    if (!machineModule) return;

    // 发射数据段
    m_asmOutput += ".data\n";
    m_asmOutput += ".align 2\n";

    // 发射文本段
    m_asmOutput += ".text\n";

    // 发射每个函数
    for (auto* func : machineModule->getFunctions()) {
        RiscVAsmEmitter emitter(m_targetMachine, func);
        m_asmOutput += emitter.emit();
        m_asmOutput += "\n";
    }
}
