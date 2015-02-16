#pragma once

#include <utility>
#include <initializer_list>

#include "AST.hpp"

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/IR/Module.h>
#include "llvm/IR/Verifier.h"

namespace pegsolitaire {
namespace codegen {

// TODO handle (or remove) the case where the generation returns nullptr

class CodeGenerator : public boost::static_visitor<llvm::Value*> {
public:
    CodeGenerator(llvm::Module *mod, llvm::IRBuilder<> & builder);

    template<class Type>
    llvm::Value* operator()(const pegsolitaire::ast::Constant<Type> & c) {
        using namespace llvm;
        //auto t = TypeBuilder<Type, false>::get(m_module->getContext());
        return ConstantInt::get(m_module->getContext(), llvm::APInt(64, c.value())); // TODO hardcoded bit-width!
    }

    template<class Type>
    llvm::Value* operator()(const pegsolitaire::ast::Binary<Type> & node) {
        auto left = boost::apply_visitor(*this, node.left);
        auto right = boost::apply_visitor(*this, node.right);
        if (!left || !right)
            return nullptr;
        switch (node.op) {
        case ast::Operator::AND:
            return m_builder.CreateAnd(left, right);
        case ast::Operator::OR:
            return m_builder.CreateOr(left, right);
        case ast::Operator::MIN: {
            // TODO how do we know it's unsigned? => type system!
            llvm::Value* tmp = m_builder.CreateICmpULT(left, right);
            return m_builder.CreateSelect(tmp, left, right);
            }
        }
        throw std::runtime_error {std::string("unknown operator: ") + static_cast<char>(node.op) };
    }

    template<class Type>
    llvm::Value* operator()(const pegsolitaire::ast::Shift<Type> & node) {
        auto x = boost::apply_visitor(*this, node.x);
        if (!x)
            return nullptr;
        if (node.numberOfBits > 0)
            return m_builder.CreateShl(x, node.numberOfBits);
        else
            return m_builder.CreateShr(x, node.numberOfBits);
    }

    template<class Type>
    llvm::Value* operator()(const pegsolitaire::ast::Variable<Type> & v) {
        if (m_variables.find(v) == m_variables.end())
            throw std::runtime_error(std::string("variable ") + v.internalName() + " not defined!");
        return m_variables.at(v);
    }

    template<class Ret, class... Types>
    llvm::Function* operator()(const pegsolitaire::ast::Function<Ret, Types...> & function) {
        // TODO check if the given function has already been generated
        using namespace pegsolitaire::ast;
        using namespace llvm;

        auto ft = TypeBuilder<Ret(Types...), false>::get(m_module->getContext());
        llvm::Function * f = llvm::Function::Create(
                    ft,
                    llvm::Function::ExternalLinkage,
                    function.internalName(),
                    m_module);
        BasicBlock * bb = BasicBlock::Create(
                    m_module->getContext(),
                    "entry",
                    f);
        m_builder.SetInsertPoint(bb);

        auto argIt = f->arg_begin();
        for (auto & v : function.args()) {
            assert(argIt != f->arg_end());
            argIt->setName(v.originalName());
            addVariable(v, &*argIt);
            argIt++;
        }
        assert(argIt == f->arg_end());

        // this has been done *after* the variables have been registered!
        auto body = function.body();
        auto retVal = boost::apply_visitor(*this, body);

        m_builder.CreateRet(retVal);
        verifyFunction(*f);
        return f;
    }

    template<class Ret>
    llvm::Value* operator()(const pegsolitaire::ast::Call<Ret> & call) {
        return nullptr; // TODO please implement!
    }

private:
    void addVariable(pegsolitaire::ast::UntypedVariable x, llvm::Value*);

    llvm::IRBuilder<> & m_builder;
    llvm::Module * m_module; // TODO garbage collected?
    std::map<ast::UntypedVariable, llvm::Value*> m_variables; // TODO garbage collected?

};

}
}
