
#ifndef KAIJU_IR_RETURNINST_H
#define KAIJU_IR_RETURNINST_H

#include "kaiju/IR/Instruction.h"

namespace kaiju {

// Class ReturnInst
//
// \brief This class is the itermediate representation for the return
// instruction.
//
class ReturnInst : public Instruction {

    // \brief The value being returned.
    //
    // If this is null than there is no return value.
    Value *RetValue;

protected:

    // ctor.
    explicit ReturnInst(Context &C, Value *RVal)
         : Instruction(C, Instruction::ReturnInstTy) {
        RetValue = RVal;
    }

public:

    // \brief Primary way of constucting a BinaryOperator object.
    static ReturnInst *get(Value *RVal) {
        return new ReturnInst(RVal->getValueType()->getContext(), RVal);
    }

    /// Methods for support type inquiry through isa, cast, and dyn_cast:
    static bool classof(const Instruction *V) {
        return V->getValueID() == Instruction::ReturnInstTy;
    }
};

} // namespace kaiju

#endif // KAIJU_IR_RETURNINST_H
