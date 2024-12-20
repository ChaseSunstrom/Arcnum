#ifndef SPARK_FUNCTION_TRAITS_HPP
#define SPARK_FUNCTION_TRAITS_HPP

#include <tuple>
#include "traits.hpp"

namespace Spark {
	// Primary template for function pointers
	template<typename R, typename... Args> struct FunctionTraitsBase {
		using ReturnType                 = R;
		using ArgsTuple                  = std::tuple<Args...>;
		static constexpr size_t Arity    = sizeof...(Args);

		template<size_t I> using ArgType = std::tuple_element_t<I, ArgsTuple>;
	};

	template<typename F> struct FunctionTraits;

	// Function pointers
	template<typename R, typename... Args> struct FunctionTraits<R (*)(Args...)> : public FunctionTraitsBase<R, Args...> {};

	// Member function pointers
	template<typename R, typename C, typename... Args> struct FunctionTraits<R (C::*)(Args...)> : public FunctionTraitsBase<R, Args...> {};

	// Const member function pointers
	template<typename R, typename C, typename... Args> struct FunctionTraits<R (C::*)(Args...) const> : public FunctionTraitsBase<R, Args...> {};

	// Function reference
	template<typename R, typename... Args> struct FunctionTraits<R (&)(Args...)> : public FunctionTraitsBase<R, Args...> {};

	// Function type
	template<typename R, typename... Args> struct FunctionTraits<R(Args...)> : public FunctionTraitsBase<R, Args...> {};

	// Generic callable (lambdas, functors)
	template<typename F> struct FunctionTraits {
	  private:
		using CallType = decltype(&F::operator());

	  public:
		using ReturnType                 = typename FunctionTraits<CallType>::ReturnType;
		using ArgsTuple                  = typename FunctionTraits<CallType>::ArgsTuple;
		static constexpr size_t Arity    = FunctionTraits<CallType>::Arity;

		template<size_t I> using ArgType = typename FunctionTraits<CallType>::template ArgType<I>;
	};

	// Handle references
	template<typename F> struct FunctionTraits<F&> : public FunctionTraits<F> {};

	template<typename F> struct FunctionTraits<F&&> : public FunctionTraits<F> {};

	template<typename F> struct FunctionTraits<const F&> : public FunctionTraits<F> {};
} // namespace Spark

#endif