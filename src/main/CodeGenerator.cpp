#include "CodeGenerator.hpp"

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
#include <llvm/Support/TargetSelect.h>

#include <set>
#include <stdexcept>

namespace pegsolitaire {
namespace codegen {
using namespace pegsolitaire::ast;

CodeGenerator::CodeGenerator(llvm::Module * module, llvm::IRBuilder<> & builder)
    : m_builder(builder)
    , m_module(module)
{}

void CodeGenerator::addVariable(UntypedVariable variable, llvm::Value* value) {
    // don't add the same variable twice (variable ought to be unique anyway)
    if (m_variables.find(variable) == m_variables.end())
        m_variables[variable] = value;
}


}
}
