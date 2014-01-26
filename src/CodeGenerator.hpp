#pragma once

#include <memory>

#include "AST.hpp"

// TODO hide this implementation detail
#include <llvm/IR/IRBuilder.h>

// forward declaration
namespace llvm {
  class Value;
  class Module;
  class Function;
  class ExecutionEngine;
}

namespace pegsolitaire {
  namespace codegen {

    // TODO handle (or remove) the case where the generation returns nullptr

    class ProgramCodeGenerator;

    class ExpressionCodeGenerator : public boost::static_visitor<llvm::Value*> {
      struct Impl;
      Impl* impl;
      friend class ProgramCodeGenerator;

    public:
      ExpressionCodeGenerator(llvm::Module *mod, llvm::IRBuilder<>& builder);
      ~ExpressionCodeGenerator();

      void setVariable(const pegsolitaire::ast::Variable & x, llvm::Value*);

      llvm::Value* operator()(const boost::dynamic_bitset<> &) const;
      llvm::Value* operator()(std::shared_ptr<pegsolitaire::ast::Binary>) const;
      llvm::Value* operator()(std::shared_ptr<pegsolitaire::ast::Shift>) const;
      llvm::Value* operator()(const pegsolitaire::ast::Variable & v) const;
    };

  }
}
