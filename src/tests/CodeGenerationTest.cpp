#define BOOST_TEST_MODULE "CodeGenerationTest"
#include "TestCommon.hpp"

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/TargetSelect.h>

#include <iostream>

#include "AST.hpp"
#include "CodeGenerator.hpp"

struct LlvmFixture {
    LlvmFixture() {
        llvm::InitializeNativeTarget();
    }
};

BOOST_GLOBAL_FIXTURE( LlvmFixture );

BOOST_AUTO_TEST_CASE(minTest) {
    using namespace pegsolitaire::ast;
    Variable<unsigned long> a {"a"};
    Variable<unsigned long> b {"b"};
    Expression<unsigned long> expr = min(a, b);
    pegsolitaire::ast::Function<unsigned long, unsigned long, unsigned long> function {"minimum", expr, a, b};

    using namespace llvm;

    Module * module = new Module("minTest", getGlobalContext());
    IRBuilder<> builder(getGlobalContext());
    pegsolitaire::codegen::CodeGenerator cg(module, builder);
    auto f = cg(function);
    //module->dump();
    ExecutionEngine * executionEngine = EngineBuilder(module).create();
    auto fp = reinterpret_cast<uint64_t(*)(u_int64_t, uint64_t)>(executionEngine->getPointerToFunction(f));
    BOOST_CHECK_EQUAL(fp(4,7), 4);
    BOOST_CHECK_EQUAL(fp(10,7), 7);
}
