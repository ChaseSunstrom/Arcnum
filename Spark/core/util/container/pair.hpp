#ifndef SPARK_PAIR_HPP
#define SPARK_PAIR_HPP

namespace Spark {
	template<typename _Ty1, typename _Ty2> struct Pair {
		using FirstType  = _Ty1;
		using SecondType = _Ty2;
		
		Pair() = default;
		Pair(const _Ty1& first, const _Ty2& second)
			: first(first)
			, second(second) {}

		_Ty1 first;
		_Ty2 second;
	};
} // namespace Spark

#endif