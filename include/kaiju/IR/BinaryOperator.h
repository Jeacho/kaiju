
#ifndef KAIJU_IR_BINARYOPERATOR_H
#define KAIJU_IR_BINARYOPERATOR_H

#include "kaiju/IR/Instruction.h"

namespace kaiju {

// Class BinaryOperator
//
// \brief This class is the itermediate representation for Binary Operation.
//
class BinaryOperator : public Instruction {

    // \brief The operation being expressed in this Binary Operation.
    BinaryOpTy Operator;

    // \brief This Binary Operation's operand.
    Value *LHS, //< Left-hand operand.
          *RHS; //< Right-hand operand.

protected:

    // ctor.
    explicit BinaryOperator(Context &C, BinaryOpTy Oper, Value *LHO, Value *RHO)
         : Instruction(C, Instruction::BinaryOpInstTy) { /* empty */ }

public:

    // \brief Primary way of constucting a BinaryOperator object.
    static BinaryOperator *get(BinaryOpTy Ty, Value *LHO, Value *RHO) {
        return new BinaryOperator(LHO->getValueType()->getContext(),
            Ty, LHO, RHO);
    }

    /// Methods for support type inquiry through isa, cast, and dyn_cast:
    static bool classof(const Instruction *V) {
        return V->getValueID() == Instruction::BinaryOpInstTy;
    }
};

} // namespace kaiju

#endif // KAIJU_IR_BINARYOPERATOR_H
