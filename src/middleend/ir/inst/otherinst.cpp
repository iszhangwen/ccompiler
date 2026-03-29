#include "otherinst.h"
#include "block.h"

void PhiInst::addIncoming(Value* val, BasicBlock* bb)
{
    addOperand(val);
    addOperand(bb);
}
std::vector<std::pair<Value*, BasicBlock*>> PhiInst::getIncoming()
{
    std::vector<std::pair<Value*, BasicBlock*>> res;
    for (int i = 0; i < getOperandsNumber(); i+=2) {
        auto var = getOperand(i);
        auto block = dynamic_cast<BasicBlock*>(getOperand(i+1));
        auto inc = std::make_pair(var, block);
        res.push_back(inc);
    }
    return res;
}


