#ifndef SPARK_PAIR_HPP
#define SPARK_PAIR_HPP

#include <core/util/classic/util.hpp>
#include <core/util/classic/traits.hpp>

namespace Spark {
	template<typename _Ty1, typename _Ty2> struct Pair {
		using FirstType   = _Ty1;
		using SecondType  = _Ty2;

		Pair()            = default;

		Pair(Pair&& other) noexcept
			: first(Move(other.first))
			, second(Move(other.second)) {}

		Pair& operator=(Pair&& other) noexcept {
			first  = Move(other.first);
			second = Move(other.second);
			return *this;
		}

		Pair(const _Ty1& first, const _Ty2& second)
			: first(first)
			, second(second) {}

		Pair(const _Ty1& first, _Ty2&& second) noexcept
			: first(first)
			, second(Move(second)) {}

		Pair(_Ty1&& first, _Ty2&& second) noexcept
			: first(Move(first))
			, second(Move(second)) {}

		_Ty1 first;
		_Ty2 second;

	};

} // namespace Spark

#endif