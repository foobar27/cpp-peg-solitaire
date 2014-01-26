#pragma once

#include <boost/variant.hpp>
#include <boost/dynamic_bitset.hpp>

#include <memory>

namespace pegsolitaire {
  namespace ast {

    enum class Operator : char { AND = '&', OR = '|' };
    std::ostream& operator<<(std::ostream& os, const Operator& op);

    struct Binary;
    struct Shift;
    using Expression = boost::variant
                            <boost::dynamic_bitset<>,
                             std::shared_ptr<Binary>,
                             std::shared_ptr<Shift>
                             >;

    struct Binary {
      Operator op;
      Expression left, right;
      Binary(Operator op, const Expression & left, const Expression & right);
    };

    struct Shift {
      Expression x;
      int numberOfBits;
      Shift(const Expression &, int);
    };

    std::shared_ptr<Binary> operator&(const Expression &, const Expression &);
    std::shared_ptr<Binary> operator|(const Expression &, const Expression &);
    std::shared_ptr<Shift> operator<<(const Expression &, int);
    //    std::shared_ptr<Binary> min(const Expression & left, const Expression & right);

  }

}
