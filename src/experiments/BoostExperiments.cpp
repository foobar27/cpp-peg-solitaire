#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/Support/TargetSelect.h>

#include <memory>
#include <iostream>
#include <typeinfo>
#include <utility>
#include <cstdint>
#include <iostream>

#include "CodeGenerator.hpp"


int main() {
    using namespace llvm;
    using namespace pegsolitaire::ast;
    using pegsolitaire::ast::Constant;

    InitializeNativeTarget();

    Variable<unsigned long> arg("arg");
    Expression<unsigned long> expr = min(Constant<unsigned long>(1ul), Constant<unsigned long>(3ul) | Constant<unsigned long>(42ul) & arg);
    pegsolitaire::ast::Function<unsigned long, unsigned long> function {"some_function", expr, arg};

    Module * module = new Module("pegsolitaire jit", getGlobalContext());

    IRBuilder<> builder(getGlobalContext());

    pegsolitaire::codegen::CodeGenerator cg(module, builder);

    auto f = cg(function);

    module->dump();

    ExecutionEngine * executionEngine = EngineBuilder(module).create();
    void * ptr = executionEngine->getPointerToFunction(f);
    auto fp = reinterpret_cast<uint64_t(*)(uint64_t)>(ptr);
    std::cout << "result=" << fp(2ul) << std::endl;

    return 0;
}
