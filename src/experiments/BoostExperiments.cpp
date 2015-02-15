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

#include "CodeGenerator.hpp"

using namespace pegsolitaire::ast;

// BOOST_FUSION_ADAPT_STRUCT(ast::Binary,
//                           (ast::Operator, op)
//                           (ast::Expression, left )
//                           (ast::Expression, right))

boost::dynamic_bitset<> operator"" _b(unsigned long long n) {
  return boost::dynamic_bitset<>(64, n);
}

int main() {
  // TODO improve the following syntax with boost::proto?
  Variable arg("arg");
  Expression expr = 3_b | Expression(42_b) & arg;

  llvm::Module * module = new llvm::Module("pegsolitaire jit", llvm::getGlobalContext());

  pegsolitaire::codegen::ProgramCodeGenerator pcg(module);
  auto f = pcg.generateFunction<uint64_t(uint64_t)>("someFunction", {arg}, expr);

  module->dump();

  return 0;
}
