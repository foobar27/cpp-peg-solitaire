#pragma once

#include <boost/variant.hpp>
#include <boost/dynamic_bitset.hpp>

#include <type_traits>
#include <vector>
#include <sstream>
#include <memory>

namespace pegsolitaire {
namespace ast {

enum class Operator : char { AND = '&', OR = '|' };
std::ostream& operator<<(std::ostream& os, const Operator& op);

using Index = long;

struct Indexed {
public:
    Indexed();
    Indexed(const Index index);

    template<class Self>
    bool operator<(const Self & other) const {
        return m_index < other.index();
    }

    Index index() const;

private:
    Index m_index;
};

struct Named {
    Named(std::string originalName);
    std::string originalName() const;
private:
    std::string m_originalName;
};

struct NamedAndIndexed : public Named, public Indexed {
    NamedAndIndexed(const std::string & originalName);
    NamedAndIndexed(const std::string & originalName, Index index);
    std::string internalName() const;
};

template<class Type>
class Constant : public Indexed {
public:

    Constant()
        : Constant<Type>( Type() )
    {}

    // TODO we need some kind of Type->NativeType conversion
    Constant(const Type & value)
        : m_value(value)
    {}

    Type value() const {
        return m_value;
    }

private:
    Type m_value;
};

template<class Type>
class Variable;

// TODO introduce more information (e.g. __LINE__)
class UntypedVariable : public NamedAndIndexed {
public:
    UntypedVariable(const std::string & originalName);
private:
    UntypedVariable(const std::string & originalName, Index index);

    template<class Type>
    friend class Variable;
};

template<class Type>
class Variable : public NamedAndIndexed {
public:
    Variable(const std::string & originalName)
        : NamedAndIndexed(originalName)
    {}

    operator UntypedVariable() const {
        return UntypedVariable(originalName(), index());
    }
};

template<class Ret>
class UntypedFunction : public NamedAndIndexed
{
    UntypedFunction(const std::string & originalName)
        : NamedAndIndexed(originalName)
    {}
};

template<class Ret, class... Types>
class Function;

template<class Ret>
class Call {
public:
    template<class... Types>
    Call(Function<Ret, Types...> f, Variable<Types>... args)
        : m_function(f)
    {
        addAllArgs(args...);
    }

private:
    void addAllArgs() {}

    template<class Type, class... Types>
    void addAllArgs(Variable<Type> arg, Variable<Types>... args) {
        m_args.push_back(arg);
        addAllArgs(args...);
    }

    UntypedFunction<Ret> m_function;
    std::vector<UntypedVariable> m_args;
};

template<class Type>
struct Binary;

template<class Type>
struct Shift;

template<class Type>
using Expression = boost::variant
  <Constant<Type>,
   boost::recursive_wrapper<Binary<Type>>,
   boost::recursive_wrapper<Shift<Type>>,
   Variable<Type>,
   Call<Type>
  >;

template<class T>
struct is_expression : std::false_type{};

template<class Type>
struct is_expression<Expression<Type>> : std::true_type {};

template<class Type>
struct is_expression<Constant<Type>> : std::true_type {};

template<class Type>
struct is_expression<Binary<Type>> : std::true_type {};

template<class Type>
struct is_expression<Shift<Type>> : std::true_type {};

template<class Type>
struct is_expression<Variable<Type>> : std::true_type {};

template<class Type>
struct is_expression<Call<Type>> : std::true_type {};

template<class T>
struct expression_type {
    using type = T;
};

template<class Type>
struct expression_type<Expression<Type>> {
    using type = Type;
};

template<class Type>
struct expression_type<Constant<Type>> {
    using type = Type;
};

template<class Type>
struct expression_type<Binary<Type>> {
    using type = Type;
};

template<class Type>
struct expression_type<Shift<Type>> {
    using type = Type;
};

template<class Type>
struct expression_type<Variable<Type>> {
    using type = Type;
};

template<class Type>
struct expression_type<Call<Type>> {
    using type = Type;
};

template<class Ret, class... Types>
class Function : public NamedAndIndexed {
public:
    using self = Function<Ret, Types...>;

    Function(const std::string name, Expression<Ret> body, Variable<Types>... args)
        : NamedAndIndexed(name)
        , m_body(body)
    {
        addAllArgs(args...);
    }

    template<class... Args>
    Call<Ret> operator()(Args... args) {
        return { *this, args... };
    }

    Expression<Ret> body() const {
        return m_body;
    }

    std::vector<UntypedVariable> args() const {
        return m_args;
    }

    operator UntypedFunction<Ret>() const {
        return {originalName(), m_body};
    }

private:

    void addAllArgs() {}

    template<class T, class... TS>
    void addAllArgs(Variable<T> arg, Variable<TS>... args) {
        m_args.push_back(arg);
        addAllArgs(args...);
    }

    Expression<Ret> m_body;
    std::vector<UntypedVariable> m_args;
};

template<class Type>
struct Binary {
    Operator op;
    Expression<Type> left, right;
    Binary(Operator op, const Expression<Type> & left, const Expression<Type> & right)
        : op(op)
        , left(left)
        , right(right)
    {}
};

template<class Type>
struct Shift {
    Expression<Type> x;
    int numberOfBits;
    Shift(const Expression<Type> & x, int numberOfBits)
        : x(x)
        , numberOfBits(numberOfBits)
    {}
};

namespace {

template<class Left, class Right>
Binary<typename expression_type<Left>::type> makeOperator(Operator op, const Left & left, const Right & right) {
    static_assert(is_expression<Left>::value, "Left must be an expression");
    static_assert(is_expression<Right>::value, "Right must be an expression");
    using left_type = typename expression_type<Left>::type;
    using right_type = typename expression_type<Right>::type;
    static_assert(std::is_same<left_type, right_type>::value, "Left and Right must be expressions of the same type");
    Expression<left_type> l = left;
    Expression<right_type> r = right;
    return { op, l, r };
}

}

template<class Left, class Right>
Binary<typename expression_type<Left>::type> operator&(const Left & left, const Right & right) {
    return makeOperator(Operator::AND, left, right);
}

template<class Left, class Right>
Binary<typename expression_type<Left>::type> operator|(const Left & left, const Right & right) {
    return makeOperator(Operator::OR, left, right);
}

template<class ExpressionType>
Shift<typename expression_type<ExpressionType>::type> operator<<(const ExpressionType & x, int numberOfBits) {
    static_assert(is_expression<ExpressionType>::value, "ExpressionType must be an expression");
    return { x, numberOfBits };
}

template<class Type>
Binary<Type> min(const Expression<Type> & left, const Expression<Type> & right);

}

}
