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

using namespace ccompiler;

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
        if (inst) {
            selectInstruction(mblock, inst);
        }
    }
}

void InstSelectPass::selectReturnInst(MachineBlock* mblock, ReturnInst* inst)
{
    // 如果有返回值，将返回值移动到 a0
    if (!inst->isVoid() && inst->getValue()) {
        Value* retVal = inst->getValue();
        int retVReg = getValueVReg(retVal);
        (void)retVReg;
        // mv a0, retVal
        auto* mvInst = createInst(RiscvInstOpcode::MV);
        mvInst->addOperand(MachineOperand::createPReg(RiscvReg::A0));
        mvInst->addOperand(MachineOperand::createVReg(retVal));
        addInst(mblock, mvInst);
    }
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
    // 为 alloca 分配栈偏移并生成地址计算
    int vreg = getValueVReg(inst);
    (void)vreg;
    int allocaSize = inst->getType()->getSize();
    if (allocaSize <= 0) allocaSize = 4; // 未知类型默认4字节

    // 栈帧从sp开始向上增长（正偏移）
    int offset = m_stackOffset;
    m_stackOffset += allocaSize;
    m_allocaOffsets[inst] = offset;

    // addi reg, sp, offset  (offset is positive, within the frame)
    auto* addrInst = createInst(RiscvInstOpcode::ADDI);
    addrInst->addOperand(MachineOperand::createVReg(inst));  // dest
    addrInst->addOperand(MachineOperand::createPReg(RiscvReg::SP));  // base = sp
    addrInst->addOperand(MachineOperand::createImm(offset)); // offset (positive)
    addInst(mblock, addrInst);
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
    (void)destVReg;

    RiscvInstOpcode opc = RiscvInstOpcode::ADD;  // 默认

    // 获取 lhs 和 rhs 的操作数
    Value* lhs = inst->getOperand(0);
    Value* rhs = inst->getOperand(1);

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
    case Instruction::Eq: {
        // xor rd, lhs, rhs;  seqz rd, rd
        // => rd = (lhs == rhs) ? 1 : 0
        auto* xorI = createInst(RiscvInstOpcode::XOR);
        xorI->addOperand(MachineOperand::createVReg(inst));
        xorI->addOperand(MachineOperand::createVReg(lhs));
        xorI->addOperand(MachineOperand::createVReg(rhs));
        addInst(mblock, xorI);
        auto* seqzI = createInst(RiscvInstOpcode::SEQZ);
        seqzI->addOperand(MachineOperand::createVReg(inst));
        seqzI->addOperand(MachineOperand::createVReg(inst));
        addInst(mblock, seqzI);
        return;
    }
    case Instruction::Ne: {
        // xor rd, lhs, rhs;  snez rd, rd
        // => rd = (lhs != rhs) ? 1 : 0
        auto* xorI = createInst(RiscvInstOpcode::XOR);
        xorI->addOperand(MachineOperand::createVReg(inst));
        xorI->addOperand(MachineOperand::createVReg(lhs));
        xorI->addOperand(MachineOperand::createVReg(rhs));
        addInst(mblock, xorI);
        auto* snezI = createInst(RiscvInstOpcode::SNEZ);
        snezI->addOperand(MachineOperand::createVReg(inst));
        snezI->addOperand(MachineOperand::createVReg(inst));
        addInst(mblock, snezI);
        return;
    }
    case Instruction::Lt:
        // slt rd, lhs, rhs  => rd = (lhs < rhs) ? 1 : 0
        opc = RiscvInstOpcode::SLT;
        break;
    case Instruction::Le: {
        // slt rd, rhs, lhs;  xori rd, rd, 1
        // => rd = (rhs < lhs) ? 1 : 0  then  rd = !rd  => rd = (lhs <= rhs) ? 1 : 0
        auto* sltI = createInst(RiscvInstOpcode::SLT);
        sltI->addOperand(MachineOperand::createVReg(inst));
        sltI->addOperand(MachineOperand::createVReg(rhs));  // 交换操作数
        sltI->addOperand(MachineOperand::createVReg(lhs));
        addInst(mblock, sltI);
        auto* xoriI = createInst(RiscvInstOpcode::XORI);
        xoriI->addOperand(MachineOperand::createVReg(inst));
        xoriI->addOperand(MachineOperand::createVReg(inst));
        xoriI->addOperand(MachineOperand::createImm(1));
        addInst(mblock, xoriI);
        return;
    }
    case Instruction::Gt: {
        // slt rd, rhs, lhs  => rd = (rhs < lhs) = (lhs > rhs)
        auto* sltI = createInst(RiscvInstOpcode::SLT);
        sltI->addOperand(MachineOperand::createVReg(inst));
        sltI->addOperand(MachineOperand::createVReg(rhs));  // 交换操作数
        sltI->addOperand(MachineOperand::createVReg(lhs));
        addInst(mblock, sltI);
        return;
    }
    case Instruction::Ge: {
        // slt rd, lhs, rhs;  xori rd, rd, 1
        // => rd = (lhs < rhs) ? 1 : 0  then  rd = !rd  => rd = (lhs >= rhs) ? 1 : 0
        auto* sltI = createInst(RiscvInstOpcode::SLT);
        sltI->addOperand(MachineOperand::createVReg(inst));
        sltI->addOperand(MachineOperand::createVReg(lhs));
        sltI->addOperand(MachineOperand::createVReg(rhs));
        addInst(mblock, sltI);
        auto* xoriI = createInst(RiscvInstOpcode::XORI);
        xoriI->addOperand(MachineOperand::createVReg(inst));
        xoriI->addOperand(MachineOperand::createVReg(inst));
        xoriI->addOperand(MachineOperand::createImm(1));
        addInst(mblock, xoriI);
        return;
    }
    default:
        opc = RiscvInstOpcode::ADD;
        break;
    }

    auto* binInst = createInst(opc);
    binInst->addOperand(MachineOperand::createVReg(inst));          // dest
    binInst->addOperand(MachineOperand::createVReg(lhs));          // lhs
    binInst->addOperand(MachineOperand::createVReg(rhs));          // rhs
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
    // 1. 初始化栈偏移跟踪（从0开始，正值向上增长）
    m_stackOffset = 0;
    m_allocaOffsets.clear();

    // 2. 构建机器级表示
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

    // 设置栈帧大小（至少16字节用于保存ra和s0，16字节对齐）
    int frameSize = m_stackOffset;
    if (frameSize < 16) frameSize = 16;
    frameSize = (frameSize + 15) & ~15;
    m_function->setFrameSize(frameSize);

    return true;
}
