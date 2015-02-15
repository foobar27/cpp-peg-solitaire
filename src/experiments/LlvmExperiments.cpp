#include <llvm/Pass.h>
#include <llvm/PassManager.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/CallingConv.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/GlobalVariable.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/MathExtras.h>
#include <algorithm>
using namespace llvm;

Module* makeLLVMModule();

int main(int argc, char**argv) {
    Module* Mod = makeLLVMModule();
    verifyModule(*Mod, PrintMessageAction);
    PassManager PM;
    PM.add(createPrintModulePass(&outs()));
    PM.run(*Mod);
    return 0;
}


Module* makeLLVMModule() {
    // Module Construction
    Module* mod = new Module("foo.bc", getGlobalContext());
    mod->setDataLayout("e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64-S128");
    mod->setTargetTriple("x86_64-unknown-linux-gnu");

    // Type Definitions
    std::vector<Type*>FuncTy_0_args;
    FuncTy_0_args.push_back(IntegerType::get(mod->getContext(), 64));
    FunctionType* FuncTy_0 = FunctionType::get(
                /*Result=*/IntegerType::get(mod->getContext(), 64),
                /*Params=*/FuncTy_0_args,
                /*isVarArg=*/false);

    PointerType* PointerTy_1 = PointerType::get(IntegerType::get(mod->getContext(), 64), 0);


    // Function Declarations

    Function* func__Z3foom = mod->getFunction("_Z3foom");
    if (!func__Z3foom) {
        func__Z3foom = Function::Create(
                    /*Type=*/FuncTy_0,
                    /*Linkage=*/GlobalValue::ExternalLinkage,
                    /*Name=*/"_Z3foom", mod);
        func__Z3foom->setCallingConv(CallingConv::C);
    }
    AttributeSet func__Z3foom_PAL;
    {
        SmallVector<AttributeSet, 4> Attrs;
        AttributeSet PAS;
        {
            AttrBuilder B;
            B.addAttribute(Attribute::NoUnwind);
            B.addAttribute(Attribute::UWTable);
            PAS = AttributeSet::get(mod->getContext(), ~0U, B);
        }

        Attrs.push_back(PAS);
        func__Z3foom_PAL = AttributeSet::get(mod->getContext(), Attrs);

    }
    func__Z3foom->setAttributes(func__Z3foom_PAL);

    // Global Variable Declarations


    // Constant Definitions
    ConstantInt* const_int32_2 = ConstantInt::get(mod->getContext(), APInt(32, StringRef("1"), 10));
    ConstantInt* const_int64_3 = ConstantInt::get(mod->getContext(), APInt(64, StringRef("255"), 10));
    ConstantInt* const_int64_4 = ConstantInt::get(mod->getContext(), APInt(64, StringRef("1"), 10));

    // Global Variable Definitions

    // Function Definitions

    // Function: _Z3foom (func__Z3foom)
    {
        Function::arg_iterator args = func__Z3foom->arg_begin();
        Value* int64_field = args++;
        int64_field->setName("field");

        BasicBlock* label_5 = BasicBlock::Create(mod->getContext(), "",func__Z3foom,0);

        // Block  (label_5)
        AllocaInst* ptr_6 = new AllocaInst(IntegerType::get(mod->getContext(), 64), "", label_5);
        ptr_6->setAlignment(8);
        AllocaInst* ptr_foo = new AllocaInst(IntegerType::get(mod->getContext(), 64), "foo", label_5);
        ptr_foo->setAlignment(8);
        StoreInst* void_7 = new StoreInst(int64_field, ptr_6, false, label_5);
        void_7->setAlignment(8);
        LoadInst* int64_8 = new LoadInst(ptr_6, "", false, label_5);
        int64_8->setAlignment(8);
        BinaryOperator* int64_9 = BinaryOperator::Create(Instruction::And, int64_8, const_int64_3, "", label_5);
        StoreInst* void_10 = new StoreInst(int64_9, ptr_foo, false, label_5);
        void_10->setAlignment(8);
        LoadInst* int64_11 = new LoadInst(ptr_foo, "", false, label_5);
        int64_11->setAlignment(8);
        BinaryOperator* int64_12 = BinaryOperator::Create(Instruction::Add, int64_11, const_int64_4, "", label_5);
        ReturnInst::Create(mod->getContext(), int64_12, label_5);

    }

    return mod;
}
