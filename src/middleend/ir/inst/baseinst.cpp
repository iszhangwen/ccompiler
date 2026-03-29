#include "baseinst.h"
#include "block.h"

const std::unordered_map<Instruction::OpCode, std::string> Instruction::Opcode2NameMap = 
{
    #define X_MACROS(a, b) {OpCode::a, b},
    INST_MACROS_TABLE
    #undef X_MACROS
};

Instruction::Instruction(OpCode opc, QualType ty, BasicBlock* parent)
: User(ty), m_opcode(opc), m_parent(parent)
{
    if (Instruction::Opcode2NameMap.count(opc)) {
        setName(Instruction::Opcode2NameMap.at(opc));
    } else {
        setName("unkown");
    }
}
