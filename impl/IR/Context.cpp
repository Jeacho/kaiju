
#include "kaiju/IR/Context.h"

using namespace kaiju;

namespace kaiju {

Context &getGlobalContext() {
    static Context globalContext;
    return globalContext;
}

}

ContextImpl::ContextImpl(Context &C)
     : Int1Ty(C, 1), Int8Ty(C, 8), Int16Ty(C, 16),
       Int32Ty(C, 32), Int64Ty(C, 64), Int128Ty(C, 128),
       VoidTy(C, Type::VoidTyID),
       HalfTy(C, Type::HalfTyID),
       FloatTy(C, Type::FloatTyID),
       DoubleTy(C, Type::DoubleTyID),
       LabelTy(C, Type::LabelTyID),
       MetadataTy(C, Type::MetadataTyID),
       TokenTy(C, Type::TokenTyID),
       IntegerTy(C, Type::IntegerTyID),
       FunctionTy(C, Type::FunctionTyID),
       StructTy(C, Type::StructTyID),
       ArrayTy(C, Type::ArrayTyID),
       PointerTy(C, Type::PointerTyID) { }

Context::Context() {
    impl = new ContextImpl(*this);
}
