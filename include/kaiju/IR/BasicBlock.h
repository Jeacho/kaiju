
#ifndef KAIJU_IR_BASICBLOCK_H
#define KAIJU_IR_BASICBLOCK_H

#include "kaiju/IR/Instruction.h"

namespace kaiju {

    class Function;

class BasicBlock : public Value {
    friend class TranslationUnit;

    // \brief This is a list of instructions within this block.
    std::vector<Instruction *> InstList;

    // \brief This is the Block's parent;
    Function *Parent;

    // ctor.
    explicit BasicBlock(Context &C, StringRef Name)
         : Value(Type::getLabelTy(C), Value::BasicBlockVal), Parent(nullptr) {
        setName(Name);
    }

public:
    BasicBlock(const BasicBlock &) = delete;
    BasicBlock &operator=(const BasicBlock &) = delete;

    // \brief The primary method for constructing new BasicBlock objects.
    static BasicBlock *get(Context &C, StringRef Name) {
        return new BasicBlock(C, Name);
    }

    /// Methods for support type inquiry through isa, cast, and dyn_cast:
    static bool classof(const Value *V) {
        return V->getValueID() == Value::BasicBlockVal;
    }
};

} // namespace kaiju

#endif // KAIJU_IR_BASICBLOCK_H
