
#ifndef KAIJU_IR_CONTEXT_H
#define KAIJU_IR_CONTEXT_H

#include "kaiju/ADT/StringRef.h"
#include "kaiju/IR/ContextImpl.h"

namespace kaiju {

    class Type;
    class Value;

class Context {
public:
    ContextImpl *impl;

    // This member tracks all name bindings to values within this context.
    std::map<const Value *, StringRef> ValueNames;

    // This member tracks all non-primitive IntegerTypes allocated within this
    // Context. Refer to IntegerType::get() for a better idea of why this member
    // exists and what it's purpose is.
    std::map<unsigned, IntegerType *> IntegerTypes;

    Context();

};

// \brief Easily accessible global context.
Context &getGlobalContext();

} // namespace kaiju

#endif // KAIJU_IR_CONTEXT_H
