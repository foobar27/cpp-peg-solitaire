#include "AST.hpp"

#include <atomic>
#include <sstream>

namespace pegsolitaire {
namespace ast {

static std::atomic<VariableIndex> nextVariableIndex {0};

Variable::Variable(const std::string & originalName)
    : m_index(nextVariableIndex++)
    , m_originalName(originalName)
{}

std::string Variable::originalName() const {
    return m_originalName;
}

std::string Variable::internalName() const {
    std::stringstream ss;
    ss << m_originalName << "_" << m_index;
    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const Operator& op) {
    os << static_cast<char>(op);
    return os;
}

Binary::Binary(Operator op, const Expression & left, const Expression & right)
    : op(op)
    , left(left)
    , right(right)
{}

Shift::Shift(const Expression & x, int numberOfBits)
    : x(x)
    , numberOfBits(numberOfBits)
{}

std::shared_ptr<Binary> operator&(const Expression & left, const Expression & right) {
    return std::shared_ptr<Binary>(new Binary(Operator::AND, left, right));
}

std::shared_ptr<Binary> operator|(const Expression & left, const Expression & right) {
    return std::shared_ptr<Binary>(new Binary(Operator::OR, left, right));
}

std::shared_ptr<Shift> operator<<(const Expression & x, int numberOfBits) {
    return std::shared_ptr<Shift>(new Shift(x, numberOfBits));
}

}
}
