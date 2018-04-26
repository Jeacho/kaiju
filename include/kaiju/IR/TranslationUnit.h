
#ifndef KAIJU_IR_TRANSLATIONUNIT_H
#define KAIJU_IR_TRANSLATIONUNIT_H

#include <map>

#include "kaiju/ADT/StringRef.h"
#include "kaiju/IR/Type.h"
#include "kaiju/IR/Function.h"
#include "kaiju/IR/Argument.h"
#include "kaiju/IR/BinaryOperator.h"
#include "kaiju/IR/ReturnInst.h"
#include "kaiju/IO/Path.h"
#include "kaiju/IO/MemoryBuffer.h"

namespace kaiju {

class TranslationUnit {

    // \brief This is a symbol table mapping all functions to their respective
    // labels.
    std::map<StringRef, Function *> FunctionSymbolTable;

    // \brief This is the path of this Translation Unit's file.
    Path &path;

    // \brief The buffer for this Translation Unit.
    MemoryBuffer &buffer;

public:
    TranslationUnit(Path &p, MemoryBuffer &mb)
        : path(p), buffer(mb) { /* empty */ };

    // \brief Returns the buffer for this TranslationUnit
    const MemoryBuffer &getMemoryBuffer() const { return buffer; }

    // \brief Returns the path of this TranslationUnit
    const Path &getPath() const { return path; }

    // \brief Primary way of constructing a itermediate function node.
    Value *createFunc(Type *Result, StringRef Name) {
        assert(Result && "Type cannot be null");

        FunctionType *ty = FunctionType::get(Result);
        Function *fn = new Function(ty, Name);
        return cast<Value>(fn);
    }

    // \brief Inserts a new argument of the specified type into the function
    // provied.
    Value *createArg(Type *Ty, Function *Fn, StringRef Name = "") {
        Argument *arg = cast<Argument>(Argument::get(Ty,
            Fn->getFunctionType()->Params.size(), Name));
        Fn->getFunctionType()->Params.push_back(arg);
        arg->Parent = Fn;

        return cast<Value>(arg);
    }

    // \brief Inserts a BasicBlock into a function.
    Value *createBlock(Context &C, StringRef Name, Function *Fn) {
        assert(!Fn->InstructionBody && "This function already has a body.");

        BasicBlock *bb = BasicBlock::get(C, Name);
        Fn->InstructionBody = bb;
        bb->Parent = Fn;

        return cast<Value>(bb);
    }

    // \brief Creates a new BinaryOperation instruction inside the block
    // specified.
    Value *createBinOp(Instruction::BinaryOpTy Ty,
            Value *LHO, Value *RHO, BasicBlock *Block) {
        BinaryOperator *BinOp = BinaryOperator::get(Ty, LHO, RHO);
        Block->InstList.push_back(BinOp);

        return cast<Value>(BinOp);
    }

    // \brief Creates a Return instruction inside the block specified.
    Value *createRet(Value *RetValue, BasicBlock *Block) {
        ReturnInst *Ret = ReturnInst::get(RetValue);
        Block->InstList.push_back(Ret);

        return cast<Value>(Ret);
    }
};

} // namespace kaiju

#endif // KAIJU_IR_TRANSLATIONUNIT_H
