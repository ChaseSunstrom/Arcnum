#ifndef SPARK_GENERAL_HPP
#define SPARK_GENERAL_HPP

#include <core/pch.hpp>
#include <core/util/container/pair.hpp>

namespace Spark {
	template<typename V1, typename V2> Pair<V1, V2> PairOf(const V1& v1, const V2& v2) { return Pair<V1, V2>(v1, v2); }

	template<typename V2> Pair<const char*, V2> PairOf(const char* v1, const V2& v2) { return Pair<const char*, V2>(v1, v2); }
} // namespace Spark

#endif