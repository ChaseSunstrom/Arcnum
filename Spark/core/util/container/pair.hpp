#ifndef SPARK_PAIR_HPP
#define SPARK_PAIR_HPP

#include <core/util/classic/traits.hpp>
#include <core/util/classic/util.hpp>

namespace Spark {
	template<typename _Ty1, typename _Ty2> struct Pair {
		using FirstType                    = _Ty1;
		using SecondType                   = _Ty2;

		Pair()                             = default;
		Pair(const Pair& other)            = default;

		Pair& operator=(const Pair& other) = default;
		Pair(Pair&&) noexcept              = default;
		Pair& operator=(Pair&&) noexcept   = default;

		template<typename _Uty1, typename _Uty2> Pair(_Uty1&& first, _Uty2&& second) noexcept
			: first(Forward<_Uty1>(first))
			, second(Forward<_Uty2>(second)) {}

		_Ty1 first;
		_Ty2 second;
	};

} // namespace Spark

#endif