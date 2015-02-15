#include "AST.hpp"

#include <atomic>
#include <sstream>

namespace pegsolitaire {
namespace ast {

static std::atomic<Index> nextIndex {0};

Index getNextIndex() {
    return nextIndex++;
}

Indexed::Indexed()
    : m_index(getNextIndex())
{}

Indexed::Indexed(Index index)
    : m_index(index)
{}

Index Indexed::index() const {
    return m_index;
}

Named::Named(std::string originalName)
    : m_originalName(originalName)
{}

std::string Named::originalName() const {
    return m_originalName;
}

NamedAndIndexed::NamedAndIndexed(const std::string & originalName)
    : Named(originalName)
{}

NamedAndIndexed::NamedAndIndexed(const std::string & originalName, Index index)
    : Named(originalName)
    , Indexed(index)
{}

std::string NamedAndIndexed::internalName() const {
    std::stringstream ss;
    ss << originalName() << "_" << index();
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Operator& op) {
    os << static_cast<char>(op);
    return os;
}

UntypedVariable::UntypedVariable(const std::string & originalName)
    : NamedAndIndexed(originalName)
{}

UntypedVariable::UntypedVariable(const std::string & originalName, Index index)
    : NamedAndIndexed(originalName, index)
{}


}
}
