
#ifndef KAIJU_IR_ARGUMENT_H
#define KAIJU_IR_ARGUMENT_H

#include "kaiju/IR/Value.h"
#include "kaiju/IR/Function.h"

namespace kaiju {

    class Function;

class Argument : public Value {
    friend class TranslationUnit;
    friend class Function;

    // \brief The function signature this argument belongs to.
    Function *Parent;

    // \brief This arguments index.
    unsigned ArgNo;

protected:
    Argument(Type *Ty, unsigned ArgNo, StringRef Name = "")
         : Value(Ty, Value::ArgumentVal), ArgNo(ArgNo), Parent(nullptr) {
        setName(Name);
    };

public:
    inline const Function *getParent() const { return Parent; }
    inline       Function *getParent()       { return Parent; }

    /// Return the index of this formal argument in its containing function.
    ///
    /// For example in "void foo(int a, float b)" a is 0 and b is 1.
    unsigned getArgNo() const {
        assert(Parent && "can't get number of unparented arg");
        return ArgNo;
    }

    // \brief This is the primary way of constructing a new Argument.
    static Value *get(Type *Ty, unsigned ArgNo, StringRef Name = "") {
        return cast<Value>(new Argument(Ty, ArgNo, Name));
    }

    /// Methods for support type inquiry through isa, cast, and dyn_cast:
    static bool classof(const Value *V) {
        return V->getValueID() == Value::ArgumentVal;
    }
};

} // namespace kaiju

#endif // KAIJU_IR_ARGUMENT_H
