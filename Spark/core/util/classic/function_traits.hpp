#ifndef SPARK_FUNCTION_TRAITS_HPP
#define SPARK_FUNCTION_TRAITS_HPP

#include <tuple>
#include "traits.hpp"

namespace Spark {
	template<typename F> struct FunctionTraits;

	// Primary template for function pointers
	template<typename R, typename... Args> struct FunctionTraits<R (*)(Args...)> {
		using ReturnType              = R;
		using TupleType               = std::tuple<Args...>;
		static constexpr size_t Arity = sizeof...(Args);
	};

	// Specialization for member function pointers
	template<typename R, typename C, typename... Args> struct FunctionTraits<R (C::*)(Args...)> {
		using ReturnType              = R;
		using ClassType               = C;
		using TupleType               = std::tuple<Args...>;
		static constexpr size_t Arity = sizeof...(Args);
	};

	// Specialization for const member function pointers
	template<typename R, typename C, typename... Args> struct FunctionTraits<R (C::*)(Args...) const> {
		using ReturnType              = R;
		using ClassType               = C;
		using TupleType               = std::tuple<Args...>;
		static constexpr size_t Arity = sizeof...(Args);
	};

	// Specialization for lambda and functors
	template<typename F> struct FunctionTraits {
	  private:
		using CallOperator = decltype(&std::remove_reference_t<F>::operator());
		using Traits       = FunctionTraits<CallOperator>;

	  public:
		using ReturnType              = typename Traits::ReturnType;
		using TupleType               = typename Traits::TupleType;
		static constexpr size_t Arity = Traits::Arity;
	};

	// Deduction guide for function pointers and references
	template<typename F> struct FunctionTraits<F&> : public FunctionTraits<F> {};

	template<typename F> struct FunctionTraits<const F&> : public FunctionTraits<F> {};

	// Utility type aliases
	template<typename F> using FunctionReturnType     = typename FunctionTraits<F>::ReturnType;

	template<typename F> using FunctionParameterTuple = typename FunctionTraits<F>::TupleType;
} // namespace Spark

#endif // SPARK_FUNCTION_TRAITS_HPP