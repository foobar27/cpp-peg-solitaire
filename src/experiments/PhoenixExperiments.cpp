#include <boost/phoenix/scope/let.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/phoenix/function.hpp>
#include <boost/phoenix/statement.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/phoenix/scope/local_variable.hpp>
#include <boost/phoenix/scope/lambda.hpp>

#include <iostream>
#include <algorithm>

struct is_odd_impl {
  using result_type = bool;
  template<typename Arg>
  bool operator()(Arg arg1) {
    return arg1 % 2 == 1;
  }

};

boost::phoenix::function<is_odd_impl> is_odd;

struct for_each_impl {
  template<typename C, typename F>
  struct result { using type = void; };

  template<typename C, typename F>
  void operator()(C& c, F f) const {
    std::for_each(c.begin(), c.end(), f);
  }
};

boost::phoenix::function<for_each_impl> for_each;

template<typename F>
void print(F f) {
  std::cout << f() << std::endl;
}

using namespace boost::phoenix;
using namespace boost::phoenix::arg_names;
using namespace boost::phoenix::local_names;

int main(int argc, char** argv) {
  int i = 4;
  print(val(3));
  print(ref(i));

  std::cout << val(5)() << std::endl;
  std::cout << val(5)(2) << std::endl;
  std::cout << arg1(6) << std::endl;
  std::cout << arg2(6, 7) << std::endl;
  std::cout << (arg1 * arg2)(3 , 4) << std::endl;
  std::cout << (arg1 * arg2)(3, 100) << std::endl;
  std::cout << construct<std::string>("hello")() << std::endl;

  struct size_key;
  expression::local_variable<size_key>::type size;

  auto f = let(_a = 3, _b = 5, size = 10) [ _a + _b + size ];
  std::cout << f() << std::endl;

  return 0;
}
