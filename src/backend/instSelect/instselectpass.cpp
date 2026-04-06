#include "instselectpass.h"
#include "../mir/machinefunction.h"
#include "../mir/machineblock.h"
#include "../mir/machineinst.h"
#include "../mir/machinemodule.h"
#include "function.h"
#include "block.h"
#include "inst/baseinst.h"
#include "inst/binaryinst.h"
#include "inst/meminst.h"
#include "inst/terminator.h"
#include "inst/otherinst.h"
#include "values/argument.h"
#include "values/constant.h"
#include "usedef.h"
#include "arena.h"

#include <unordered_map>

// 下一个虚拟寄存器编号
static int nextVReg = 1;

int InstSelectPass::allocVReg(Value* val)
{
    int vreg = nextVReg++;
    val->setVReg(vreg);
    m_function->addVirtualRegister(vreg);
    return vreg;
}

int InstSelectPass::getValueVReg(Value* val)
{
    if (auto* arg = dynamic_cast<Argument*>(val)) {
        if (arg->getVReg() < 0) {
            return allocVReg(val);
        }
        return arg->getVReg();
    }
    if (val->getVReg() < 0) {
        return allocVReg(val);
    }
    return val->getVReg();
}

void InstSelectPass::selectBlock(MachineBlock* mblock, BasicBlock* block)
{
    // 处理基本块中的所有指令
    for (auto inst : block->getInsts()) {
        selectInstruction(mblock, inst);
    }
}

void InstSelectPass::selectReturnInst(MachineBlock* mblock, ReturnInst* inst)
{
    // ret指令
    auto* retInst = createInst(RiscvInstOpcode::RET);
    addInst(mblock, retInst);
}

void InstSelectPass::selectBranchInst(MachineBlock* mblock, BranchInst* inst)
{
    BasicBlock* ssaBlock = mblock->getSSABlock();
    if (!ssaBlock) return;

    auto successors = ssaBlock->getSuccessors();

    if (inst->isUnconditional()) {
        // 无条件跳转 - 跳转到第一个后继
        if (!successors.empty()) {
            auto* dest = successors.front();
            auto* brInst = createInst(RiscvInstOpcode::JAL);
            brInst->addOperand(MachineOperand::createPReg(RiscvReg::ZERO));
            brInst->addOperand(MachineOperand::createLabel(dest->getName()));
            addInst(mblock, brInst);
        }
    } else {
        // 条件跳转 - ifThen是第一个后继，ifElse是第二个后继
        Value* cond = inst->getCond();
        int condVReg = getValueVReg(cond);

        // 获取后继基本块
        BasicBlock* thenBlock = nullptr;
        BasicBlock* elseBlock = nullptr;

        if (successors.size() >= 1) {
            thenBlock = *successors.begin();
        }
        if (successors.size() >= 2) {
            elseBlock = *(++successors.begin());
        }

        // 生成条件跳转
        if (thenBlock && elseBlock) {
            // beq cond, zero, elseBlock  => if(cond != 0) goto elseBlock
            auto* brInst = createInst(RiscvInstOpcode::BNEZ);
            brInst->addOperand(MachineOperand::createVReg(cond));
            brInst->addOperand(MachineOperand::createLabel(thenBlock->getName()));
            addInst(mblock, brInst);

            // 无条件跳转到elseBlock
            auto* jmpInst = createInst(RiscvInstOpcode::JAL);
            jmpInst->addOperand(MachineOperand::createPReg(RiscvReg::ZERO));
            jmpInst->addOperand(MachineOperand::createLabel(elseBlock->getName()));
            addInst(mblock, jmpInst);
        }
    }
}

void InstSelectPass::selectLoadInst(MachineBlock* mblock, LoadInst* inst)
{
    Value* addr = inst->getAddr();
    int destVReg = getValueVReg(inst);
    int addrVReg = getValueVReg(addr);

    // 简单的lw加载
    auto* loadInst = createInst(RiscvInstOpcode::LW);
    loadInst->addOperand(MachineOperand::createVReg(inst));     // dest (use Value*)
    loadInst->addOperand(MachineOperand::createVReg(addr));     // base (use Value*)
    loadInst->addOperand(MachineOperand::createImm(0));        // offset
    addInst(mblock, loadInst);
}

void InstSelectPass::selectStoreInst(MachineBlock* mblock, StoreInst* inst)
{
    Value* val = inst->getValue();
    Value* addr = inst->getAddr();
    int valVReg = getValueVReg(val);
    int addrVReg = getValueVReg(addr);

    // 简单的sw存储
    auto* storeInst = createInst(RiscvInstOpcode::SW);
    storeInst->addOperand(MachineOperand::createVReg(val));     // value (use Value*)
    storeInst->addOperand(MachineOperand::createVReg(addr));    // base (use Value*)
    storeInst->addOperand(MachineOperand::createImm(0));        // offset
    addInst(mblock, storeInst);
}

void InstSelectPass::selectAllocaInst(MachineBlock* mblock, AllocaInst* inst)
{
    // alloca在函数 prologue 处理，这里只是记录
    // 实际的栈分配在帧信息中处理
}

void InstSelectPass::selectPhiInst(MachineBlock* mblock, PhiInst* inst)
{
    // 对于简单实现，PHI指令会在后续处理中消除
    // 这里先为结果分配虚拟寄存器
    int destVReg = getValueVReg(inst);
    (void)destVReg;  // 避免未使用警告
}

void InstSelectPass::selectBinaryInst(MachineBlock* mblock, BinaryInst* inst)
{
    int destVReg = getValueVReg(inst);

    RiscvInstOpcode opc = RiscvInstOpcode::ADD;  // 默认

    switch (inst->getOpCode()) {
    case Instruction::Add:
        opc = RiscvInstOpcode::ADD;
        break;
    case Instruction::Sub:
        opc = RiscvInstOpcode::SUB;
        break;
    case Instruction::Mul:
        opc = RiscvInstOpcode::MUL;
        break;
    case Instruction::Div:
        opc = RiscvInstOpcode::DIV;
        break;
    case Instruction::Mod:
        opc = RiscvInstOpcode::REM;
        break;
    case Instruction::Shl:
        opc = RiscvInstOpcode::SLL;
        break;
    case Instruction::Lshr:
        opc = RiscvInstOpcode::SRL;
        break;
    case Instruction::Ashr:
        opc = RiscvInstOpcode::SRA;
        break;
    case Instruction::And:
        opc = RiscvInstOpcode::AND;
        break;
    case Instruction::Or:
        opc = RiscvInstOpcode::OR;
        break;
    case Instruction::Xor:
        opc = RiscvInstOpcode::XOR;
        break;
    case Instruction::Eq:
        // sgt/slt + seqz 实现
        opc = RiscvInstOpcode::SLTU;
        break;
    case Instruction::Ne:
        opc = RiscvInstOpcode::SLTU;
        break;
    case Instruction::Lt:
        opc = RiscvInstOpcode::SLT;
        break;
    case Instruction::Le:
        opc = RiscvInstOpcode::SLT;
        break;
    case Instruction::Gt:
        opc = RiscvInstOpcode::SLT;
        break;
    case Instruction::Ge:
        opc = RiscvInstOpcode::SLT;
        break;
    default:
        opc = RiscvInstOpcode::ADD;
        break;
    }

    auto* binInst = createInst(opc);
    binInst->addOperand(MachineOperand::createVReg(inst));          // dest
    binInst->addOperand(MachineOperand::createVReg(inst->getOperand(0))); // lhs
    binInst->addOperand(MachineOperand::createVReg(inst->getOperand(1))); // rhs
    addInst(mblock, binInst);
}

void InstSelectPass::selectInstruction(MachineBlock* mblock, Instruction* inst)
{
    if (!inst) return;

    // 根据指令类型进行选择
    switch (inst->getOpCode()) {
    case Instruction::Alloca: {
        auto* allocaInst = static_cast<AllocaInst*>(inst);
        selectAllocaInst(mblock, allocaInst);
        break;
    }
    case Instruction::Load: {
        auto* loadInst = static_cast<LoadInst*>(inst);
        selectLoadInst(mblock, loadInst);
        break;
    }
    case Instruction::Store: {
        auto* storeInst = static_cast<StoreInst*>(inst);
        selectStoreInst(mblock, storeInst);
        break;
    }
    case Instruction::Ret: {
        auto* retInst = static_cast<ReturnInst*>(inst);
        selectReturnInst(mblock, retInst);
        break;
    }
    case Instruction::Branch: {
        auto* brInst = static_cast<BranchInst*>(inst);
        selectBranchInst(mblock, brInst);
        break;
    }
    case Instruction::Phi: {
        auto* phiInst = static_cast<PhiInst*>(inst);
        selectPhiInst(mblock, phiInst);
        break;
    }
    case Instruction::Add:
    case Instruction::Sub:
    case Instruction::Mul:
    case Instruction::Div:
    case Instruction::Mod:
    case Instruction::Shl:
    case Instruction::Lshr:
    case Instruction::Ashr:
    case Instruction::And:
    case Instruction::Or:
    case Instruction::Xor:
    case Instruction::Eq:
    case Instruction::Ne:
    case Instruction::Lt:
    case Instruction::Le:
    case Instruction::Gt:
    case Instruction::Ge: {
        auto* binInst = static_cast<BinaryInst*>(inst);
        selectBinaryInst(mblock, binInst);
        break;
    }
    default:
        // 未处理的指令，添加注释
        break;
    }
}

bool InstSelectPass::runOnFunction(Function* node)
{
    // 1. 构建机器级表示
    m_function = Arena::make<MachineFunction>(node);
    m_function->setName(node->getName());
    m_function->setFrameInfo(m_targetMachine->getRISCFrameInfo());
    m_machineModule.addFunction(m_function);

    // 2. 创建每个SSA函数的基本块对应的机器基本块
    for (auto block : node->getBasicBlocks()) {
        auto mblock = Arena::make<MachineBlock>(block);
        mblock->setLabel(block->getName());
        m_function->addBlock(mblock);
    }

    // 3. 遍历每个基本块进行指令选择
    auto ssaBlocks = node->getBasicBlocks();
    auto mBlocks = m_function->getBlocks();
    auto ssaIt = ssaBlocks.begin();
    auto mIt = mBlocks.begin();

    for (; ssaIt != ssaBlocks.end() && mIt != mBlocks.end(); ++ssaIt, ++mIt) {
        selectBlock(*mIt, *ssaIt);
    }

    return true;
}
