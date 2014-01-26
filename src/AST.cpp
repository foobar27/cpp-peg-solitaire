#include "AST.hpp"

namespace pegsolitaire {
  namespace ast {

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
