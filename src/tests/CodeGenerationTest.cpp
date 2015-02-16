#define BOOST_TEST_MODULE "CodeGenerationTest"
#include "TestCommon.hpp"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>
#include <functional>

#include "AST.hpp"
#include "CodeGenerator.hpp"

struct LlvmFixture {
    LlvmFixture() {
        llvm::InitializeNativeTarget();
    }
};

BOOST_GLOBAL_FIXTURE( LlvmFixture )

template<class Ret, class... Types>
Ret(*compile(const pegsolitaire::ast::Function<Ret, Types...> & function))(Types...) {
    using namespace llvm;
    // uint64_t(*)(u_int64_t, uint64_t)
    Module * module = new Module("compiledModule", getGlobalContext());
    IRBuilder<> builder(getGlobalContext());
    pegsolitaire::codegen::CodeGenerator cg(module, builder);
    auto f = cg(function);
    //module->dump();
    ExecutionEngine * executionEngine = EngineBuilder(module).create();
    // TODO we actually need a mapping from Types... to NativeTypes... here:
    return reinterpret_cast<Ret(*)(Types...)>(executionEngine->getPointerToFunction(f));
}

BOOST_AUTO_TEST_CASE(minTest) {

    using namespace pegsolitaire::ast;

    Variable<unsigned long> a {"a"};
    Variable<unsigned long> b {"b"};
    Function<unsigned long, unsigned long, unsigned long> function = {"minimum", min(a, b), a, b};

    auto fp = compile(function);

    BOOST_CHECK_EQUAL(fp(4,7), 4);
    BOOST_CHECK_EQUAL(fp(10,7), 7);
}
