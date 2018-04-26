
#ifndef KAIJU_IR_CONTEXTIMPL_H
#define KAIJU_IR_CONTEXTIMPL_H

#include "kaiju/IR/DerivedTypes.h"

namespace kaiju {

class ContextImpl {
    friend class Context;
    friend class Type;
    friend class IntegerType;

    // Standard width IntegerTypes
    IntegerType Int1Ty;     //< 1-bit width integer type.
    IntegerType Int8Ty;     //< 8-bit width integer type.
    IntegerType Int16Ty;    //< 16-bit width integer type.
    IntegerType Int32Ty;    //< 32-bit width integer type.
    IntegerType Int64Ty;    //< 64-bit width integer type.
    IntegerType Int128Ty;   //< 128-bit width integer type.

    Type VoidTy;
    Type HalfTy;
    Type FloatTy;
    Type DoubleTy;
    Type LabelTy;
    Type MetadataTy;
    Type TokenTy;
    Type IntegerTy;
    Type FunctionTy;
    Type StructTy;
    Type ArrayTy;
    Type PointerTy;

public:
    // Ctor.
    ContextImpl(Context &C);

};

} // namespace kaiju

#endif // KAIJU_IR_CONTEXTMPL_H
