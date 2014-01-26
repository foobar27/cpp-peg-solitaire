#include <boost/fusion/sequence.hpp>
#include <boost/fusion/include/sequence.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/include/pair.hpp>
#include <boost/fusion/algorithm.hpp>
#include <boost/fusion/include/algorithm.hpp>
#include <boost/type_traits/is_pointer.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/TypeBuilder.h>
#include <llvm/Support/TargetSelect.h>

#include <memory>
#include <iostream>
#include <typeinfo>
#include <utility>
#include <cstdint>

#include "CodeGenerator.hpp"

// TODO replace recursive_wrapper by std::unique_ptr
// TODO operator overloading
// TODO use spirit::karma for generating xml

//namespace fusion = boost::fusion;

using namespace pegsolitaire::ast;

// class ExpressionPrinter : public boost::static_visitor<> {
// public:
//   void operator()(const boost::dynamic_bitset<> & i) const {
//     std::cout << i.to_ulong();
//   }
//   void operator()(std::shared_ptr<Binary> node) const {
//     std::cout << "(" << node->op << " ";
//     boost::apply_visitor(*this, node->left);
//     std::cout << " ";
//     boost::apply_visitor(*this, node->right);
//     std::cout << ")";
//   }
//   void operator()(std::shared_ptr<Shift> node) const {
//     std::cout << "(<< ";
//     boost::apply_visitor(*this, node->x);
//     std::cout << " " << node->numberOfBits << ")";
//   }
//   void operator()(const Variable & variable) const {
//     std::cout << variable.internalName();
//   }
// };

// BOOST_FUSION_ADAPT_STRUCT(ast::Binary,
//                           (ast::Operator, op)
//                           (ast::Expression, left )
//                           (ast::Expression, right))

boost::dynamic_bitset<> operator"" _b(unsigned long long n) {
  return boost::dynamic_bitset<>(64, n);
}

int main() {
  // TODO improve the following syntax with boost::proto
  Variable arg("arg");
  Expression expr = 3_b | Expression(42_b) & arg;

  llvm::Module * module = new llvm::Module("pegsolitaire jit", llvm::getGlobalContext());

  auto ft = llvm::TypeBuilder<uint64_t(uint64_t), false>::get(module->getContext());
  llvm::Function *f = llvm::Function::Create
    (ft,
     llvm::Function::ExternalLinkage,
     "getNormalForm",
     module);
  f->arg_begin()->setName("fc");

  llvm::IRBuilder<> builder(module->getContext());
  pegsolitaire::codegen::ExpressionCodeGenerator cg(module, builder);
  llvm::BasicBlock *bb = llvm::BasicBlock::Create
    (module->getContext(),
     "entry",
     f);
  builder.SetInsertPoint(bb);

  cg.setVariable(arg, &*f->arg_begin());
  auto x = boost::apply_visitor(cg, expr);
  //  x->dump();

  llvm::Value* retVal = x;
  builder.CreateRet(retVal);
  llvm::verifyFunction(*f);

  module->dump();

  //for_each(foo, print_xml());

  // using fusion::at_c;
  // fusion::vector<int, char, std::string> stuff(1, 'x', "howdy");
  // int i = fusion::at_c<0>(stuff);
  // char ch = at_c<1>(stuff);
  // std::string s = at_c<2>(stuff);
  //
  // std::cout << s << std::endl;
  // xml_print_pointers(stuff);
  // std::cout << std::endl;
  //
  // //  using namespace fields;
  // using fusion::at_key;
  // using fusion::for_each;
  // person a_person;
  // std::string person_name = at_key<name>(a_person);
  // int person_age = at_key<age>(a_person);
  // fusion::for_each(a_person, print_xml());

  return 0;
}

// namespace fields {
//   struct name;
//   struct age;
// }
//
// using person = fusion::map
//   <fusion::pair<fields::name, std::string>,
//    fusion::pair<fields::age, std::string>
//    >;
//
// struct print_xml {
//   template <typename T>
//   void operator()(T const& x) const {
//     std::cout
//       << '<' << typeid(x).name() << '>'
//       << x
//       << "</" << typeid(x).name() << '>'
//       << std::endl;
//     ;
//   }
// };
//
// struct print_json {
//   template<typename T>
//   void operator()(T const& x) const {
//     std::cout << "\"" << typeid(x).name() << "\": "
//               << "\"" << x << "\"" << std::endl;
//   };
// };
//
// template <typename Sequence>
// void xml_print_pointers(Sequence const& seq)
// {
//   fusion::for_each(fusion::filter_if<boost::is_integral<mpl_::_> >(seq),
//                    print_xml());
// }


// /*=============================================================================
//   Copyright (c) 2002-2010 Joel de Guzman
//
//   Distributed under the Boost Software License, Version 1.0. (See accompanying
//   file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//   =============================================================================*/
// ///////////////////////////////////////////////////////////////////////////////
// //
// //  This sample demontrates a parser for a comma separated list of numbers.
// //  No actions.
// //
// //  [ JDG May 10, 2002 ]    spirit1
// //  [ JDG March 24, 2007 ]  spirit2
// //
// ///////////////////////////////////////////////////////////////////////////////
//
// #include <boost/config/warning_disable.hpp>
// #include <boost/spirit/include/qi.hpp>
// #include <boost/fusion/include/adapt_struct.hpp>
// #include <boost/fusion/include/io.hpp>
//
// #include <iostream>
// #include <string>
// #include <vector>
//
//
// Set[_1 + _2 + _3 | _1 <= set1, _2 <= bag2, guard(_1 > _2), _3 <= List[_1 <= list3]]
//
//
// namespace client
// {
//   namespace qi = boost::spirit::qi;
//   namespace ascii = boost::spirit::ascii;
//
//   struct ones_ : qi::symbols<char, unsigned>
//   {
//     ones_()
//     {
//       add
//         ("I"    , 1)
//         ("II"   , 2)
//         ("III"  , 3)
//         ("IV"   , 4)
//         ("V"    , 5)
//         ("VI"   , 6)
//         ("VII"  , 7)
//         ("VIII" , 8)
//         ("IX"   , 9)
//         ;
//     }
//
//   } ones;
//
//
//   template <typename Iterator>
//   bool parse_numbers(Iterator first, Iterator last, std::vector<double> & v) {
//     using qi::double_;
//     using qi::phrase_parse;
//     using ascii::space;
//     using ascii::space_type;
//
//     //    qi::rule<Iterator, space_type> r = (double_ | ones) % ',';
//
//     bool result = phrase_parse(first,
//                                last,
//                                double_ >> *(',' >> double_),
//                                space,
//                                v
//                                );
//     if (first != last) // fail if we did not get a full match
//       return false;
//     return result;
//   }
//
//   struct employee
//   {
//     int age;
//     std::string surname;
//     std::string forename;
//     double salary;
//   };
//
// }
//
//
// BOOST_FUSION_ADAPT_STRUCT(
//                           client::employee,
//                           (int, age)
//                           (std::string, surname)
//                           (std::string, forename)
//                           (double, salary));
//
// namespace client {
//
//   template <typename Iterator>
//   struct employee_parser : qi::grammar<Iterator, employee(), ascii::space_type> {
//     employee_parser()
//       : employee_parser::base_type(start) {
//       using qi::int_;
//       using qi::lit;
//       using qi::double_;
//       using qi::lexeme;
//       using ascii::char_;
//
//       quoted_string %= lexeme['"' >> +(char_ - '"') >> '"'];
//
//       start %=
//         lit("employee")
//         >> '{'
//         >>  int_ >> ','
//         >>  quoted_string >> ','
//         >>  quoted_string >> ','
//         >>  double_
//         >>  '}'
//         ;
//     }
//
//     qi::rule<Iterator, std::string(), ascii::space_type> quoted_string;
//     qi::rule<Iterator, employee(), ascii::space_type> start;
//   };
//
// }
//
// using namespace std;
//
// int
// main()
// {
//   std::cout << "/////////////////////////////////////////////////////////\n\n";
//   std::cout << "\t\tAn employee parser for Spirit...\n\n";
//   std::cout << "/////////////////////////////////////////////////////////\n\n";
//
//   std::cout
//     << "Give me an employee of the form :"
//     << "employee{age, \"surname\", \"forename\", salary } \n";
//   std::cout << "Type [q or Q] to quit\n\n";
//
//   using boost::spirit::ascii::space;
//   typedef std::string::const_iterator iterator_type;
//   typedef client::employee_parser<iterator_type> employee_parser;
//
//   employee_parser g; // Our grammar
//   std::string str;
//   while (getline(std::cin, str))
//     {
//       if (str.empty() || str[0] == 'q' || str[0] == 'Q')
//         break;
//
//       client::employee emp;
//       std::string::const_iterator iter = str.begin();
//       std::string::const_iterator end = str.end();
//       bool r = phrase_parse(iter, end, g, space, emp);
//
//       if (r && iter == end)
//         {
//           std::cout << boost::fusion::tuple_open('[');
//           std::cout << boost::fusion::tuple_close(']');
//           std::cout << boost::fusion::tuple_delimiter(", ");
//
//           std::cout << "-------------------------\n";
//           std::cout << "Parsing succeeded\n";
//           std::cout << "got: " << boost::fusion::as_vector(emp) << std::endl;
//           std::cout << "\n-------------------------\n";
//         }
//       else
//         {
//           std::cout << "-------------------------\n";
//           std::cout << "Parsing failed\n";
//           std::cout << "-------------------------\n";
//         }
//     }
//
//   std::cout << "Bye... :-) \n\n";
//   return 0;
// }
//
//
// int main_old(int argc, char **argv)
// {
//   cout << "/////////////////////////////////////////////////////////\n\n";
//   cout << "\t\tA comma separated list parser for Spirit...\n\n";
//   cout << "/////////////////////////////////////////////////////////\n\n";
//
//   cout << "Give me a comma separated list of numbers.\n";
//   cout << "Type [q or Q] to quit\n\n";
//
//   string str;
//   while (getline(cin, str)) {
//     if (str.empty() || str[0] == 'q' || str[0] == 'Q')
//       break;
//
//     vector<double> v;
//     if (client::parse_numbers(str.begin(), str.end(), v)) {
//       cout << "-------------------------\n";
//       cout << "Parsing succeeded\n";
//       cout << str << " Parses OK: " << endl;
//
//       for (vector<double>::size_type i = 0; i < v.size(); ++i)
//         cout << i << ": " << v[i] << endl;
//     }
//     else {
//       cout << "-------------------------\n";
//       cout << "Parsing failed\n";
//       cout << "-------------------------\n";
//     }
//   }
//
//   cout << "Bye... :-) \n\n";
//   return 0;
// }

// #include <iostream>
// #include <boost/proto/proto.hpp>
// #include <boost/typeof/std/ostream.hpp>
// #include <boost/dynamic_bitset.hpp>
//
// using namespace boost;
//
// // Define a pow_fun function object
// template< int Exp >
// struct pow_fun
// {
//   typedef double result_type;
//
//   double operator()(double d) const
//   {
//     return std::pow(d, Exp);
//   }
// };
//
// // Define a literal integer Proto expression.
// //proto::terminal<int>::type i = {0};
// proto::literal<int> i = 0;
//
// // "sin" is a Proto terminal containing a function pointer
// proto::terminal< double(*)(double) >::type const sinus = {&std::sin};
//
// // Define a lazy pow() function for the calculator EDSL.
// // Can be used as: pow< 2 >(_1)
// template< int Exp, typename Arg >
// typename proto::result_of::make_expr<
//   proto::tag::function  // Tag type
//   , pow_fun< Exp >        // First child (by value)
//   , Arg const &           // Second child (by reference)
//   >::type const
// pow(Arg const &arg)
// {
//   return proto::make_expr<proto::tag::function>
//     (
//      pow_fun<Exp>(),   // First child (by value)
//      boost::ref(arg)   // Second child (by reference)
//      );
// }
//
// struct calculator_domain;
//
// int main(int argc, char** argv) {
//   // This creates an expression template.
//   i + 1;
//
//   double pi = 3.1415926535;
//   proto::default_context ctx;
//   // Create a lazy "sin" invocation and immediately evaluate it
//   std::cout << proto::eval( sinus(pi/2), ctx ) << std::endl;
//   std::cout << proto::eval( pow<2>(4), ctx);
//
//   return 0;
// }



// proto::terminal< std::ostream & >::type cout_ = { std::cout };
//
// // Define a placeholder type
// template<int I>
// struct placeholder
// {};
//
// struct calculator_context
//   : proto::callable_context<calculator_context const>
// {
//   // Values to replace the placeholders
//   std::vector<double> args;
//
//   // Define the result type of the calculator.
//   // (This makes the calculator_context "callable".)
//   using result_type = double;
//
//   // Handle the placeholders:
//   template<int I>
//   double operator()(proto::tag::terminal, placeholder<I>) const {
//     return this->args[I];
//   }
// };
//
// // Forward-declare an expression wrapper
// template<typename Expr>
// struct calculator;
//
// // Define the grammar of calculator expressions
// struct calculator_grammar
//   : proto::or_<
//         proto::plus< calculator_grammar, calculator_grammar >
//       , proto::minus< calculator_grammar, calculator_grammar >
//       , proto::multiplies< calculator_grammar, calculator_grammar >
//       , proto::divides< calculator_grammar, calculator_grammar >
//       , proto::terminal< proto::_ >
//     >
// {};
//
// // Define a calculator domain. Expression within
// // the calculator domain will be wrapped in the
// // calculator<> expression wrapper.
// struct calculator_domain
//   : proto::domain< proto::generator<calculator>,
//                    calculator_grammar
//                    >
// {};
//
// // Define a calculator expression wrapper. It behaves just like
// // the expression it wraps, but with an extra operator() member
// // function that evaluates the expression.
// template<typename Expr>
// struct calculator
//   : proto::extends<Expr, calculator<Expr>, calculator_domain>
// {
//   using base_type = proto::extends<Expr, calculator<Expr>, calculator_domain>;
//
//   calculator(Expr const &expr = Expr())
//     : base_type(expr)
//   {}
//
//   using result_type = double;
//
//   // Overload operator() to invoke proto::eval() with
//   // our calculator_context.
//   double operator()(double a1 = 0, double a2 = 0) const
//   {
// //    BOOST_MPL_ASSERT((proto::matches<Expr, calculator_grammar>));
//     calculator_context ctx;
//     ctx.args.push_back(a1);
//     ctx.args.push_back(a2);
//
//     return proto::eval(*this, ctx);
//   }
// };
//
// // Define the Protofied placeholder terminals, in the
// // calculator domain.
// calculator<proto::terminal<placeholder<0> >::type> const _1;
// calculator<proto::terminal<placeholder<1> >::type> const _2;
//
// int main(int argc, char** argv) {
//   //  dynamic_bitset<> foo(8, 0);
//   //  _1 & foo;
//
//   calculator_context ctx;
//   ctx.args.push_back(45); // the value of _1 is 45
//   ctx.args.push_back(50); // the value of _2 is 50
//
//   // Create an arithmetic expression and immediately evaluate it
//   double d = proto::eval( (_2 - _1) / _2 * 100, ctx );
//   double result = ((_2 - _1) / _2 * 100)(45.0, 50.0);
//
//   std::cout << d
//             << " "
//             << result
//             << std::endl;
//
//   double a1[4] = { 56, 84, 37, 69 };
//   double a2[4] = { 65, 120, 60, 70 };
//   double a3[4] = { 0 };
//
//   // Use std::transform() and a calculator expression
//   // to calculate percentages given two input sequences:
//   _1 + 2 & 3;
//   std::transform(a1, a1+4, a2, a3, (_2 - _1) / _2 * 100 );
//
//   return 0;
// }
