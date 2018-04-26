
#ifndef KAIJU_IR_FUNCTION_H
#define KAIJU_IR_FUNCTION_H

#include "kaiju/IR/Value.h"
#include "kaiju/IR/DerivedTypes.h"
#include "kaiju/IR/BasicBlock.h"
#include "kaiju/IR/Argument.h"

namespace kaiju {

class Function : public Value {
    friend class TranslationUnit;

    // \brief This is a vector of all the Instruction classes that belong within
    // the scope of this function.
    BasicBlock *InstructionBody;

    // \brief Assertion to ensure type provided in the Function constructor
    // is a FunctionType.
    static inline Type *assertType(Type *Ty) {
        assert(isa<FunctionType>(Ty)
            && "Type provided must be a function type.");
        return Ty;
    }

    // ctor.
    Function(Type *Ty, StringRef name = "")
         : Value(assertType(Ty), Value::FunctionVal) {
        InstructionBody = nullptr;
        setName(name);
    };

public:
    // Returns the FunctionType for me.
    FunctionType *getFunctionType() const {
        return cast<FunctionType>(getValueType());
    }

    // \brief Returns this function's return type.
    Type *getReturnType() const {
        return getFunctionType()->getReturnType();
    }

    // Methods for support type inquiry through isa, cast, and dyn_cast:
    static bool classof(const Value *V) {
        return V->getValueID() == Value::FunctionVal;
    }
};

} // namespace kaiju

#endif // KAIJU_IR_FUNCTION_H
