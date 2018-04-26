
#ifndef KAIJU_IR_INSTRUCTION_H
#define KAIJU_IR_INSTRUCTION_H

#include "kaiju/IR/Value.h"

namespace kaiju {

// Class Instruction
//
// \brief This is the base class for instructions within the Kaiju IR.
//
class Instruction : public Value {
public:
    // \brief Various Binary Operations that can be expressed.
    enum BinaryOpTy {
        Add,
        Sub,
        Mul,
        Div,
        Rem,
    };

    // \brief The kind of instruction used as RTTI.
    enum InstructionTy {
        BinaryOpInstTy,
        ReturnInstTy,
    };

protected:

    // \brief RTTI.
    InstructionTy SubclassID;

    // ctor for subcasses.
    explicit Instruction(Context &C, InstructionTy Subclass)
         : Value(Type::getLabelTy(C), Value::InstructionVal),
           SubclassID(Subclass) { /* empty */ }

public:

    // \brief Returns this object's RTTI.
    InstructionTy getInstructionID() const { return SubclassID; }
};

} // namespace kaiju

#endif // KAIJU_IR_INSTRUCTION_H
