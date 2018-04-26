
#ifndef KAIJU_IR_VALUE_H
#define KAIJU_IR_VALUE_H

#include "kaiju/ADT/StringRef.h"
#include "kaiju/Support/Casting.h"
#include "kaiju/IR/Type.h"

namespace kaiju {

    class Type;

class Value {
public:
    // \brief Concrete subclass of this.
    //
    // An enumeration for keeping track of the concrete subclass of Value that
    // is actually instantiated. Values of this enumeration are kept in the
    // Value classes SubclassID field. They are used for concrete type
    // identification.
    enum ValueTy {
        FunctionVal,

        ArgumentVal,
        BasicBlockVal,

        InstructionVal,

        ConstantIntVal,
        ConstantFPVal,
    };

private:

    // \brief This value's type.
    Type *ValueType;

    const unsigned SubclassID; // Subclass identifier (for isa/dyn_cast)

    unsigned hasName : 1;   // Whether a name is bound to this Value or not.

public:
    explicit Value(Type *Ty, unsigned scid)
         : ValueType(Ty), SubclassID(scid) { /* empty */ }


    // \brief Returns whether this Value has a named bound to it or not.
    bool hasNameBinding() const { return hasName;   }

    /// \brief Return a constant reference to the value's name.
    ///
    /// This guaranteed to return the same reference as long as the value is not
    /// modified.  If the value has a name, this does a hashtable lookup, so
    /// it's not free.
    StringRef getName() const;

    /// \brief Change the name of the value.
    ///
    /// Choose a new unique name if the provided name is taken.
    ///
    /// \param Name The new name; or "" if the value's name should be removed.
    void setName(StringRef name);

    // \brief Returns the type that is paired with this value.
    Type *getValueType() const { return ValueType; }

    unsigned getValueID() const {
        return SubclassID;
    }
};

} // namespace kaiju

#endif // KAIJU_IR_VALUE_H
