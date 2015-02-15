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
// TODO using namespace llvm?

struct ExpressionCodeGenerator::Impl {
    llvm::IRBuilder<>& builder;
    llvm::Module *module;
    std::map<Variable, llvm::Value*> variables;

    Impl(llvm::Module * m, llvm::IRBuilder<>& builder)
        : builder(builder)
        , module(m)
    {}
};

ExpressionCodeGenerator::ExpressionCodeGenerator(llvm::Module *mod, llvm::IRBuilder<>& builder)
    : impl(new Impl(mod, builder))
{}

ExpressionCodeGenerator::~ExpressionCodeGenerator() {
    delete impl;
}

void ExpressionCodeGenerator::setVariable(const Variable & variable, llvm::Value* value) {
    // don't add the same variable twice (variable ought to be unique anyway)
    if (impl->variables.find(variable) == impl->variables.end())
        impl->variables[variable] = value;
}

llvm::Value* ExpressionCodeGenerator::operator()(const boost::dynamic_bitset<> & i) const {
    return llvm::ConstantInt::get(impl->module->getContext(), llvm::APInt(64, i.to_ulong()));
}

llvm::Value* ExpressionCodeGenerator::operator()(std::shared_ptr<Binary> node) const {
    auto left = boost::apply_visitor(*this, node->left);
    auto right = boost::apply_visitor(*this, node->right);
    if (!left || !right)
        return nullptr;
    switch (node->op) {
    case ast::Operator::AND:
        return impl->builder.CreateAnd(left, right);
    case ast::Operator::OR:
        return impl->builder.CreateOr(left, right);
    }
    throw std::runtime_error {std::string("unknown operator: ") + static_cast<char>(node->op) };
}

llvm::Value* ExpressionCodeGenerator::operator()(std::shared_ptr<Shift> node) const {
    auto x = boost::apply_visitor(*this, node->x);
    if (!x)
        return nullptr;
    if (node->numberOfBits > 0)
        return impl->builder.CreateShl(x, node->numberOfBits);
    else
        return impl->builder.CreateLShr(x, node->numberOfBits);
}

llvm::Value* ExpressionCodeGenerator::operator()(const Variable & v) const {
    if (impl->variables.find(v) == impl->variables.end())
        throw std::runtime_error(std::string("variable ") + v.internalName() + " not defined!");
    return impl->variables[v];
}

ProgramCodeGenerator::ProgramCodeGenerator(llvm::Module* module)
    : m_module(module)
    , m_builder(module->getContext())
{}


}
}
