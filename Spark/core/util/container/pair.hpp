#ifndef SPARK_PAIR_HPP
#define SPARK_PAIR_HPP

namespace Spark {
	template<typename T1, typename T2> struct Pair {
		using FirstType  = T1;
		using SecondType = T2;
		
		Pair() = default;
		Pair(const T1& first, const T2& second)
			: first(first)
			, second(second) {}

		T1 first;
		T2 second;
	};
} // namespace Spark

#endif