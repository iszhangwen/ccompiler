/*
ssa ir是TAC的变种形式
ssa ir指令系统包含一下几个方面：
1. 内存访问指令
2. terminator指令：意思是基本块的终结包含了ret,br指令
3.
4.
5. PHI指令
*/
#pragma once

#include "usedef.h"

class BasicBlock;

class Instruction : public User
{
public:
    // 参考llvm IR设计
    enum OpCode {
        // basic block 终止指令
        ret,
        branch,
        // binary operators
        add,
        sub,
        mul,
        div,
        // 浮点数
        float_add,
        float_sub,
        float_mul,
        float_div,
        // Memory operators
        alloca,
        load,
        store,
        // other operators
        cmp,
        float_cmp,
        phi,
        call,
        getelementptr,
        // zero extend
        zero_extend,
        // 类型转化
        float2int,
        int2float,
    };

    // 所属基本块
    void setParent(BasicBlock* parent) {m_basicBlock = parent;}
    BasicBlock* getParent() {return m_basicBlock;}

    // 指令信息
    OpCode getOpCode() {return m_opcode;}
    void setOpCode(OpCode op) {m_opcode = op;}
    int getID() const {return m_insID;};
    void setID(int id) {m_insID = id;}

    // 终结指令
    virtual bool isTerminator() const {return false;}
protected:
    Instruction(OpCode opcode, QualType ty, const std::string& name, unsigned numOps, BasicBlock* parent = nullptr)
    : User(ty, name, numOps) {}

private:
    int m_insID;
    OpCode m_opcode;
    BasicBlock* m_basicBlock;
};

// -----------------------------内存访问指令-------------------
// 从栈上分配一段内存，大小依据type决定
class AllocaInst : public Instruction
{
public:
    AllocaInst(QualType ty, BasicBlock* bb = nullptr)
    : Instruction(OpCode::alloca, ty, "alloca", 0, bb) {}

};
// 从地址获取值
class LoadInst : public Instruction
{
public:
    LoadInst(QualType ty, std::shared_ptr<Value> ptr, BasicBlock* bb = nullptr)
    : Instruction(OpCode::load, ty, "load", 1, bb) {
        setOperand(0, ptr);
    }

    std::shared_ptr<Value> getValue() {return getOperand(0);}
};
// 将值存入到地址
class StoreInst : public Instruction
{
public:
    StoreInst(QualType ty, std::shared_ptr<Value> val, std::shared_ptr<Value> ptr, BasicBlock* bb = nullptr)
    : Instruction(OpCode::store, ty, "store", 2, bb) {
        setOperand(0, val);
        setOperand(0, ptr);
    }

    std::shared_ptr<Value> getValue() {return getOperand(0);}
    std::shared_ptr<Value> getAddr() {return getOperand(1);}
};

//----------------------BinaryInst------------------------------------------
class BinaryInst : public Instruction
{
public:
    static std::shared_ptr<BinaryInst> createAdd(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block);
    static std::shared_ptr<BinaryInst> createSub(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block);
    static std::shared_ptr<BinaryInst> createMul(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block);
    static std::shared_ptr<BinaryInst> createDiv(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block);
    static std::shared_ptr<BinaryInst> createFAdd(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block);
    static std::shared_ptr<BinaryInst> createFSub(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block);
    static std::shared_ptr<BinaryInst> createFMul(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block);
    static std::shared_ptr<BinaryInst> createFDiv(std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* block);

    explicit BinaryInst(OpCode opc, std::shared_ptr<Value> v1, std::shared_ptr<Value> v2, BasicBlock* bb);
};

class CmpInst : public Instruction
{
public:
    enum opCode {
        EQ,
        NE,
        GT,
        GE,
        LT,
        LE,
    };
    
private:
    OpCode m_opCode;
};


//------------------------函数终止指令----------------------

class TerminatorInst : public Instruction
{
public:
    TerminatorInst(OpCode opcode, QualType ty, const std::string& name, unsigned numOps, BasicBlock* parent = nullptr)
    : Instruction(OpCode::ret, ty, name, numOps, parent) {}
    // 终结指令
    virtual bool isTerminator() const {return true;}
};

class ReturnInst : public TerminatorInst
{
public:
    ReturnInst(std::shared_ptr<Value> val, BasicBlock* bb = nullptr)
    : TerminatorInst(OpCode::ret, QualType(), "ret", 1, bb) {
        if (val) setOperand(0, val);
    }
    std::shared_ptr<Value> getValue() {
        return getOperand(0);
    }
};

class BranchInst : public TerminatorInst
{
public:
    // 有条件跳转构造: 跳转目标是then块或者else块
    BranchInst(BasicBlock* parent, std::shared_ptr<Value> cond, BasicBlock* ifThen, BasicBlock* ifElse);
    // 无条件跳转构造：跳转目标只有一个块
    BranchInst(BasicBlock* parent, BasicBlock* dest);

    // 判断是否是无条件跳转还是有条件跳转
    bool isUnconditional() const {return getNumOperands() == 1;}
    bool isConditional()   const {return getNumOperands() == 3;}

    std::shared_ptr<Value> getCond() {
        if (isConditional())
            return getOperand(1);
        return nullptr;
    }
    void setCond(std::shared_ptr<Value> val) {
        if (isConditional())
            setOperand(0, val);
    }
};