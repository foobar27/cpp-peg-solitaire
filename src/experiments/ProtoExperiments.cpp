#include <boost/proto/proto.hpp>
#include <boost/typeof/std/ostream.hpp>

#include <typeinfo>
#include <iostream>
#include <vector>
#include <cmath>
#include <type_traits>

using namespace boost;

proto::terminal<std::ostream&>::type cout_ = { std::cout };
proto::terminal<std::istream&>::type cin_ = { std::cin };
proto::terminal<double(*)(double)>::type const sin_ = {&std::sin};

template<int i>
struct placeholder {};

template<typename Expr>
void evaluate(const Expr & expr) {
  proto::default_context ctx;
  proto::eval(expr, ctx);
}

template<int Exp>
struct pow_fun {
  using result_type = double;
  result_type operator()(double d) const{
    return std::pow(d, Exp);
  }
};

template<int Exp, typename Arg>
const typename proto::result_of::make_expr<proto::tag::function,
                                           pow_fun<Exp>,
                                           const Arg &>::type
pow(const Arg & arg) {
  return proto::make_expr<proto::tag::function>(pow_fun<Exp>(),
                                                boost::ref(arg));
                                           }


template<typename Expr>
struct calculator;

struct calculator_grammar
  : proto::or_<proto::plus<calculator_grammar, calculator_grammar>,
               proto::minus<calculator_grammar, calculator_grammar>,
               proto::multiplies<calculator_grammar, calculator_grammar>,
               proto::divides<calculator_grammar, calculator_grammar>,
               proto::terminal<proto::_>
               >
{};

struct calculator_domain
  : proto::domain<proto::generator<calculator>, calculator_grammar>
{};

struct calculator_context
  : proto::callable_context<const calculator_context>
{
  std::vector<double> args;
  using result_type = double;
  template<int I>
  double operator()(proto::tag::terminal, placeholder<I>) const {
    return args[I];
  }
};

template<typename Expr>
struct calculator
  : proto::extends<Expr, calculator<Expr>, calculator_domain>
{
  // BOOST_PROTO_BASIC_EXTENDS(Expr, calculator<Expr>, calculator_domain)
  // BOOST_PROTO_EXTENDS_SUBSCRIPT()
  // BOOST_PROTO_EXTENDS_ASSIGN()

  using base_type = proto::extends<Expr, calculator<Expr>, calculator_domain>;
  using result_type = double;

  calculator(const Expr & expr = Expr())
    : base_type(expr)
  {}

  double operator()(double a1 = 0, double a2 = 0) const {
    calculator_context ctx;
    ctx.args.push_back(a1);
    ctx.args.push_back(a2);
    return proto::eval(*this, ctx);
  }

};

const calculator<proto::terminal<placeholder<0>>::type> _1 = {{}};
const calculator<proto::terminal<placeholder<1>>::type> _2 = {{}};

template<typename Expr>
void check_plus_node(const Expr & ) {
  static_assert(std::is_same<typename proto::tag_of<Expr>::type, proto::tag::plus>::value, "plus");
  static_assert(proto::arity_of<Expr>::value == 2, "arity");
};

void foo() {
  proto::terminal<int>::type i = {42};
  proto::terminal<int>::type & ri = proto::child_c<0>(i + 2);
}

struct display {
  template<typename T>
  void operator()(const T & t) const {
    std::cout << t << std::endl;
  }
};

struct fun_t {};
proto::terminal<fun_t>::type const fun = {{}};

struct Input
  : proto::shift_right<proto::terminal<std::istream &>, proto::_>
{};

struct Output
  : proto::shift_left<proto::terminal<std::ostream &>, proto::_>
{};

// template<typename Expr>
// void input_output(const Expr & expr) {
//   if (proto::matches<Expr, Input>::value) {
//     std::cout << "Input!" << std::endl;
//   }
//   if (proto::matches<Expr, Output>::value) {
//     std::cout << "Output!"<< std::endl;
//   }
// }

// template<typename Expr>
// void input_output(const Expr & expr);

template<typename Expr>
std::enable_if<proto::matches<Expr, Input>::type>
input_output(const Expr & expr) {
  std::cout << "Input!" << std::endl;
}

template<typename Expr>
std::enable_if<proto::matches<Expr, Output>::type>
input_output(const Expr & expr) {
  std::cout << "Output!" << std::endl;
}


int main(int argc, char** argv) {
  //evaluate(cout_ << "Hello" << " World " << (_1 + _2 * 3));
  calculator_context ctx;
  ctx.args.push_back(2);
  ctx.args.push_back(3);
  //  double d = proto::eval(_1 + _2, ctx);
  double d = (pow<2>(_1 + _2))(2, 4);
  std::cout << "d=" << d << std::endl;
  //std::cout << typeid(sin_(_1 + _2 * _1 / _1)).name() << std::endl;

  auto ex = proto::deep_copy(proto::lit(1) + 2);
  check_plus_node(ex);

  proto::display_expr(pow<2>(_1 + _2));

  fusion::for_each(fusion::transform(fusion::pop_front(fun(1,2,3,4)), proto::functional::value()), display());
  //  fusion::for_each(fusion::transform(proto::flatten(_1 + 2 + 3 + 4), proto::functional::value()), display());

  proto::display_expr(cout_ << "foo");
  input_output(cout_ << "foo");
  input_output(cin_ >> d);

  return 0;
}
