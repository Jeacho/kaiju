
#ifndef KAIJU_IR_DERIVEDTYPES_H
#define KAIJU_IR_DERIVEDTYPES_H

#include "kaiju/IR/Type.h"

namespace kaiju {

    class Argument;

// Class IntegerType
//
// A class that represents an arbitrary-width integer.
//
class IntegerType : public Type {
    friend class ContextImpl;
    friend class Context;

    unsigned width : 8; //< The width of this inteer type.

protected:
    // Ctor.
    explicit IntegerType(Context &C, unsigned width)
         : Type(C, Type::IntegerTyID) {
        this->width = width;
    }

public:
    // \brief A method for dumping the contents of this function into a
    // output stream.
    std::ostream &dump(std::ostream &os) const {
        return os << "i" << width;
    }

    static IntegerType *getInt1Ty(Context &C)   ;
    static IntegerType *getInt8Ty(Context &C)   ;
    static IntegerType *getInt16Ty(Context &C)  ;
    static IntegerType *getInt32Ty(Context &C)  ;
    static IntegerType *getInt64Ty(Context &C)  ;
    static IntegerType *getInt128Ty(Context &C) ;

    // \brief primary way of constructing IntegerType classes, used rather than
    // the vanilla constructor to ensure no redundant types are constructed.
    static IntegerType *get(Context &C, unsigned width);

    /// Methods for support type inquiry through isa, cast, and dyn_cast.
    static bool classof(const Type *T) {
        return T->getTypeID() == IntegerTyID;
    }
};

// Class FunctionType
//
// \brief Type subclass representing a Function object's type information.
//
class FunctionType : public Type {
    friend class TranslationUnit;

    // \brief This is the return type of this function signature.
    Type *Result;

    // \brief This is this functions type signature.
    std::vector<Argument *> Params;

    // ctor.
    FunctionType(Context &C, Type *Ret)
         : Type(C, Type::FunctionTyID) {
        Result = Ret;
    };

public:
    FunctionType(const FunctionType &) = delete;
    FunctionType &operator=(const FunctionType &) = delete;

    // \brief A method for dumping the contents of this function into a
    // output stream.
    std::ostream &dump(std::ostream &os) const {
        return Result->dump(os);
    }

    // \brief primary way of constructing FunctionType classes, used rather than
    // the vanilla constructor to ensure no redundant types are constructed.
    static FunctionType *get(Type *Result);

    // \brief Gets the return type of this functino signature.
    Type *getReturnType() const { return Result;}

    // \brief Returns a pointer to the parameter at the specified index.
    Argument *getParam(unsigned i) const { return Params[i]; }

    // \briefs Gets the number of parameters in this signature.
    std::size_t getNumParams() const { return Params.size(); }

    /// Methods for support type inquiry through isa, cast, and dyn_cast.,
    static bool classof(const Type *T) {
        return T->getTypeID() == FunctionTyID;
    }
};

} // namespace kaiju

#endif // KAIJU_IR_DERIVEDTYPES_H
