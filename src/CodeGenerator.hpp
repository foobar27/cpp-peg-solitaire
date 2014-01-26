#pragma once

#include <memory>

#include "AST.hpp"

// forward declaration
namespace llvm {
  class Value;
  class Module;
  class Function;
  class ExecutionEngine;
}

namespace pegsolitaire {
  namespace codegen {

    class ExpressionCodeGenerator : public boost::static_visitor<llvm::Value*> {
      struct Impl;
      Impl* impl;

    public:
      ExpressionCodeGenerator(llvm::Module *mod);
      ~ExpressionCodeGenerator();

      llvm::Value* operator()(const boost::dynamic_bitset<> &) const;
      llvm::Value* operator()(std::shared_ptr<pegsolitaire::ast::Binary>) const;
      llvm::Value* operator()(std::shared_ptr<pegsolitaire::ast::Shift>) const;
    };

  }
}
