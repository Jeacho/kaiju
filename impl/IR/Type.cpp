
#include "kaiju/IR/Context.h"
#include "kaiju/IR/Type.h"
#include "kaiju/IR/DerivedTypes.h"

using namespace kaiju;

Type *Type::getVoidTy(Context &C)       { return &C.impl->VoidTy;         }
Type *Type::getHalfTy(Context &C)       { return &C.impl->HalfTy;         }
Type *Type::getFloatTy(Context &C)      { return &C.impl->FloatTy;        }
Type *Type::getDoubleTy(Context &C)     { return &C.impl->DoubleTy;       }
Type *Type::getLabelTy(Context &C)      { return &C.impl->LabelTy;        }
Type *Type::getMetadataTy(Context &C)   { return &C.impl->MetadataTy;     }
Type *Type::getTokenTy(Context &C)      { return &C.impl->TokenTy;        }
Type *Type::getIntegerTy(Context &C)    { return &C.impl->IntegerTy;      }
Type *Type::getFunctionTy(Context &C)   { return &C.impl->FunctionTy;     }
Type *Type::getStructTy(Context &C)     { return &C.impl->StructTy;       }
Type *Type::getArrayTy(Context &C)      { return &C.impl->ArrayTy;        }
Type *Type::getPointerTy(Context &C)    { return &C.impl->PointerTy;      }

// \brief A method for dumping the contents of this function into a
// output stream.
std::ostream &Type::dump(std::ostream &os) const {
    switch (ID) {
    case TypeID::VoidTyID:      return os << "void";
    case TypeID::HalfTyID:      return os << "f16";
    case TypeID::FloatTyID:     return os << "f32";
    case TypeID::DoubleTyID:    return os << "f64";
    case TypeID::LabelTyID:     return os << "label";
    case TypeID::IntegerTyID:
        return cast<IntegerType>(this)->dump(os);

    case TypeID::FunctionTyID:
        return cast<FunctionType>(this)->dump(os);

    /* TODO:
    case TypeID::StructTyID:
    case TypeID::ArrayTyID:
    case TypeID::PointerTyID:
    */
    }
}

// \brief Checks if the type specified is a Number type.
bool Type::isNumberType(Type *ty) {
    if (ty == nullptr)
        return false;

    switch (ty->getTypeID()) {
    case TypeID::VoidTyID:
    case TypeID::LabelTyID:
    case TypeID::MetadataTyID:
    case TypeID::TokenTyID:
    case TypeID::FunctionTyID:
    case TypeID::StructTyID:
    case TypeID::ArrayTyID:
    case TypeID::PointerTyID:
        return false;

    case TypeID::HalfTyID:
    case TypeID::FloatTyID:
    case TypeID::DoubleTyID:
    case TypeID::IntegerTyID:
        return true;

    }
}

IntegerType *IntegerType::getInt1Ty(Context &C)   { return &C.impl->Int1Ty;      }
IntegerType *IntegerType::getInt8Ty(Context &C)   { return &C.impl->Int8Ty;      }
IntegerType *IntegerType::getInt16Ty(Context &C)  { return &C.impl->Int16Ty;     }
IntegerType *IntegerType::getInt32Ty(Context &C)  { return &C.impl->Int32Ty;     }
IntegerType *IntegerType::getInt64Ty(Context &C)  { return &C.impl->Int64Ty;     }
IntegerType *IntegerType::getInt128Ty(Context &C) { return &C.impl->Int128Ty;    }

// \brief This method is used to create new IntegerType instances in place of
// the IntegerType ctor. This is so redundant IntegerTypes are now created.
IntegerType *IntegerType::get(Context &C, unsigned width) {
    switch (width) {
    case 1:     return &C.impl->Int1Ty;
    case 8:     return &C.impl->Int8Ty;
    case 16:    return &C.impl->Int16Ty;
    case 32:    return &C.impl->Int32Ty;
    case 64:    return &C.impl->Int64Ty;
    case 128:   return &C.impl->Int128Ty;
    default:
        break;
    }

    IntegerType *IntType = C.IntegerTypes[width];

    if (!IntType) {
        IntType = new IntegerType(C, width);
        C.IntegerTypes.insert(std::make_pair(width, IntType));
    }

    return IntType;
}

// \brief primary way of constructing FunctionType classes, used rather than
// the vanilla constructor to ensure no redundant types are constructed.
FunctionType *FunctionType::get(Type *Result) {
    return new FunctionType(Result->getContext(), Result);
}
