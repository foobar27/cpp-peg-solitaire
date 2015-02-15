#pragma once

#include <utility>

#include "AST.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/IR/Module.h>
#include "llvm/IR/Verifier.h"

namespace pegsolitaire {
namespace codegen {

// TODO handle (or remove) the case where the generation returns nullptr

class ProgramCodeGenerator;

class ExpressionCodeGenerator : public boost::static_visitor<llvm::Value*> {
    struct Impl; // TODO no more needed to hide these details
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

class ProgramCodeGenerator {
public:
    ProgramCodeGenerator(llvm::Module*);

    template<typename FType>
    llvm::Function* generateFunction(const std::string & name,
                                     std::initializer_list<ast::Variable> variables,
                                     const ast::Expression & expression) {
        using namespace pegsolitaire::ast;
        using namespace llvm;
        auto ft = TypeBuilder<FType, false>::get(m_module->getContext());
        Function *f = Function::Create
                (ft,
                 Function::ExternalLinkage,
                 name,
                 m_module);
        auto argIt = f->arg_begin();
        ExpressionCodeGenerator cg(m_module, m_builder);
        for (auto & v : variables) {
            assert(argIt != f->arg_end());
            argIt->setName(v.internalName());
            cg.setVariable(v, &*argIt);
            argIt++;
        }
        assert(argIt == f->arg_end());
        llvm::BasicBlock *bb = llvm::BasicBlock::Create
                (m_module->getContext(),
                 "entry",
                 f);
        m_builder.SetInsertPoint(bb);
        auto retVal = boost::apply_visitor(cg, expression);
        m_builder.CreateRet(retVal);
        llvm::verifyFunction(*f);
        // TODO in case of failure: remove from parent
        return f;
    }

private:
    llvm::Module *m_module;
    llvm::IRBuilder<> m_builder;
};

}
}
