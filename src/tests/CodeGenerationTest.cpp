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

    Module * module = new Module("compiledModule", getGlobalContext());
    IRBuilder<> builder(getGlobalContext());
    pegsolitaire::codegen::CodeGenerator cg(module, builder);
    auto f = cg(function);
#ifndef NDEBUG
    module->dump();
#endif
    ExecutionEngine * executionEngine = EngineBuilder(module).create();
    // TODO we actually need a mapping from Types... to NativeTypes... here:
    return reinterpret_cast<Ret(*)(Types...)>(executionEngine->getPointerToFunction(f));
}

BOOST_AUTO_TEST_CASE( orTest ) {
    using namespace pegsolitaire::ast;

    Variable<unsigned long> a {"a"};
    Variable<unsigned long> b {"b"};
    Function<unsigned long, unsigned long, unsigned long> function {"or", a | b, a, b};
    auto fp = compile(function);

    BOOST_CHECK_EQUAL(fp(3,4), 3|4);
    BOOST_CHECK_EQUAL(fp(3,5), 3|5);
    BOOST_CHECK_EQUAL(fp(1,6), 1|6);
}

BOOST_AUTO_TEST_CASE( andTest ) {
    using namespace pegsolitaire::ast;

    Variable<unsigned long> a {"a"};
    Variable<unsigned long> b {"b"};
    Function<unsigned long, unsigned long, unsigned long> function {"or", a & b, a, b};
    auto fp = compile(function);

    BOOST_CHECK_EQUAL(fp(3,4), 3&4);
    BOOST_CHECK_EQUAL(fp(3,5), 3&5);
    BOOST_CHECK_EQUAL(fp(1,6), 1&6);
}

pegsolitaire::ast::Function<unsigned long, unsigned long> shift(int n) {
    using namespace pegsolitaire::ast;

    Variable<unsigned long> a {"a"};
    return { "shift", a << n , a};
}

BOOST_AUTO_TEST_CASE( shiftTest ) {
    auto shl1 = compile(shift(1));
    BOOST_CHECK_EQUAL(shl1(1), 2);
    BOOST_CHECK_EQUAL(shl1(2), 4);

    auto shl2 = compile(shift(2));
    BOOST_CHECK_EQUAL(shl2(1), 4);

    auto shr1 = compile(shift(-1));
    BOOST_CHECK_EQUAL(shr1(2), 1);
    BOOST_CHECK_EQUAL(shl1(8), 4);
    BOOST_CHECK_EQUAL(shr1(1), 0);

    auto shr2 = compile(shift(2));
    BOOST_CHECK_EQUAL(shr2(4), 1);

    auto shl0 = compile(shift(0));
    BOOST_CHECK_EQUAL(shl0(42), 42);
}

BOOST_AUTO_TEST_CASE( minTest ) {
    using namespace pegsolitaire::ast;

    Variable<unsigned long> a {"a"};
    Variable<unsigned long> b {"b"};
    Function<unsigned long, unsigned long, unsigned long> function {"minimum", min(a, b), a, b};
    auto fp = compile(function);

    BOOST_CHECK_EQUAL(fp(4,7), 4);
    BOOST_CHECK_EQUAL(fp(10,7), 7);
}
