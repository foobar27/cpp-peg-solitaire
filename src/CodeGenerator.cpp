#include "CodeGenerator.hpp"

#include <llvm/IR/DerivedTypes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/Support/TargetSelect.h>

#include <stdexcept>

namespace pegsolitaire {
  namespace codegen {
    using namespace pegsolitaire::ast;

    struct ExpressionCodeGenerator::Impl {
      llvm::IRBuilder<> builder;
      llvm::Module *module;

      Impl(llvm::Module * m)
        : builder(llvm::getGlobalContext())
        , module(m)
      {}
    };

    ExpressionCodeGenerator::ExpressionCodeGenerator(llvm::Module *mod)
      : impl(new Impl(mod))
    {}

    ExpressionCodeGenerator::~ExpressionCodeGenerator() {
      delete impl;
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

  }
}
