
#ifndef KAIJU_IR_TYPE_H
#define KAIJU_IR_TYPE_H

#include <map>
#include <vector>

#include "kaiju/Support/Casting.h"

namespace kaiju {

    class ContextImpl;
    class Context;
    class PointerType;
    class FunctionType;
    class IntegerType;

// Class Type
//
// \brief This is the base class for all types in kaiju's type system.
class Type {
public:
    friend class ContextImpl;
    friend class Context;

    enum TypeID {
        VoidTyID = 0,    ///<  0: type with no size
        HalfTyID,        ///<  1: 16-bit floating point type
        FloatTyID,       ///<  2: 32-bit floating point type
        DoubleTyID,      ///<  3: 64-bit floating point type
        LabelTyID,       ///<  4: Labels
        MetadataTyID,    ///<  5: Metadata
        TokenTyID,       ///<  6: Tokens

        IntegerTyID,     ///<  7: Arbitrary bit width integers
        FunctionTyID,    ///<  8: Functions
        StructTyID,      ///<  9: Structures
        ArrayTyID,       ///< 10: Arrays
        PointerTyID,     ///< 11: Pointers
    };

    static Type *getVoidTy(Context &C)     ;
    static Type *getHalfTy(Context &C)     ;
    static Type *getFloatTy(Context &C)    ;
    static Type *getDoubleTy(Context &C)   ;
    static Type *getLabelTy(Context &C)    ;
    static Type *getMetadataTy(Context &C) ;
    static Type *getTokenTy(Context &C)    ;
    static Type *getIntegerTy(Context &C)  ;
    static Type *getFunctionTy(Context &C) ;
    static Type *getStructTy(Context &C)   ;
    static Type *getArrayTy(Context &C)    ;
    static Type *getPointerTy(Context &C)  ;

protected:
    // The context that allocated this type.
    Context &Ctx;

    // RTTI.
    TypeID ID;

    explicit Type(Context &C, TypeID scid)
        : Ctx(C), ID(scid) { /* empty */ }
    ~Type() = default;

public:
    // \brief A method for dumping the contents of this function into a
    // output stream.
    virtual std::ostream &dump(std::ostream &os) const;

    // \brief Returns this type's context.
    const Context &getContext() const { return Ctx; }
          Context &getContext()       { return Ctx; }

    /// Return the type id for the type. This will return one of the TypeID enum
    /// elements defined above.
    TypeID getTypeID() const { return ID; }

    // \brief Checks if the type specified is a Number type.
    static bool isNumberType(Type *ty);
};

// allow isa<PointerType>(x) to work without DerivedTypes.h included.
template <> struct isa_impl<PointerType, Type> {
  static inline bool doit(const Type &Ty) {
    return Ty.getTypeID() == Type::PointerTyID;
  }
};

} // kaiju

#endif /// KAIJU_IR_TYPE_H
