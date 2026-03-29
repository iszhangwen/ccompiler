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
#include <string>
#include <unordered_map>

class BasicBlock;

#ifdef INST_MACROS_TABLE
#undef INST_MACROS_TABLE
#endif
#define INST_MACROS_TABLE \
    X_MACROS(Alignof, "alignof")\
    X_MACROS(Ret, "Ret")\
    X_MACROS(Branch, "Branch")\
    X_MACROS(Add, "Add")\
    X_MACROS(Sub, "Sub")\
    X_MACROS(Mul, "Mul")\
    X_MACROS(Div, "Div")\
    X_MACROS(Mod, "Mod")\
    X_MACROS(Shl, "Shl")\
    X_MACROS(Lshr, "Lshr")\
    X_MACROS(Ashr, "Ashr")\
    X_MACROS(And, "And")\
    X_MACROS(Or, "Or")\
    X_MACROS(Xor, "Xor")\
    X_MACROS(Eq, "Eq")\
    X_MACROS(Ne, "Ne")\
    X_MACROS(Lt, "Lt")\
    X_MACROS(Le, "Le")\
    X_MACROS(Gt, "Gt")\
    X_MACROS(Ge, "Ge")\
    X_MACROS(Ult, "Ult")\
    X_MACROS(Ule, "Ule")\
    X_MACROS(Ugt, "Ugt")\
    X_MACROS(Uge, "Uge")\
    X_MACROS(LogicalAnd, "LogicalAnd")\
    X_MACROS(LogicalOr, "LogicalOr")\
    X_MACROS(FAdd, "FAdd")\
    X_MACROS(FSub, "FSub")\
    X_MACROS(FMul, "FMul")\
    X_MACROS(FDiv, "FDiv")\
    X_MACROS(FEq, "FEq")\
    X_MACROS(FNe, "FNe")\
    X_MACROS(FLt, "FLt")\
    X_MACROS(FLe, "FLe")\
    X_MACROS(FGt, "FGt")\
    X_MACROS(FGe, "FGe")\
    X_MACROS(Assign, "Assign")\
    X_MACROS(AddAssign, "AddAssign")\
    X_MACROS(SubAssign, "SubAssign")\
    X_MACROS(MulAssign, "MulAssign")\
    X_MACROS(DivAssign, "DivAssign")\
    X_MACROS(Alloca, "Alloca")\
    X_MACROS(Load, "Load")\
    X_MACROS(Store, "Store")\
    X_MACROS(Gep, "Gep")\
    X_MACROS(Phi, "Phi")\
    X_MACROS(Call, "Call")\
    X_MACROS(Zext, "Zext")\
    X_MACROS(Sext, "Sext")\
    X_MACROS(Trunc, "Trunc")\
    X_MACROS(FP2SI, "FP2SI")\
    X_MACROS(SI2FP, "SI2FP")\
    X_MACROS(Max, "Max")\
    X_MACROS(Min, "Min")\

class Instruction : public User
{
public:
    // 参考llvm IR设计
    enum OpCode {
        #define X_MACROS(a, b) a,
        INST_MACROS_TABLE
        #undef X_MACROS
    };

    // @brief: 设置所属基本块
    void setParent(BasicBlock* parent) {m_parent = parent;}
    // @brief: 获取所属基本块
    BasicBlock* getParent() {return m_parent;}
    // @brief: 获取指令码
    OpCode getOpCode() const {return m_opcode;}
    // @brief: 判断是否是终结指令
    virtual bool isTerminator() const {return false;}

    // @brief: 判断是否是allOcaa指令
    bool isAlloca() const {return getOpCode() == Instruction::Alloca;}

protected:
    Instruction(OpCode opcode, QualType ty, BasicBlock* parent = nullptr);
    // @brief: 通过opcode获取name
    static const std::unordered_map<OpCode, std::string> Opcode2NameMap;

private:
    OpCode m_opcode;
    BasicBlock* m_parent;
};




