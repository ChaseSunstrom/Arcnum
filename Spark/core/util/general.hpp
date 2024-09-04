#ifndef SPARK_GENERAL_HPP
#define SPARK_GENERAL_HPP

#include <core/pch.hpp>

namespace Spark {
template <typename V1, typename V2>
std::pair<V1, V2> PairOf(const V1& v1, const V2& v2) {
	return std::make_pair(v1, v2);
}

template <typename V2>
std::pair<const char*, V2> PairOf(const char* v1, const V2& v2) {
	return std::make_pair(v1, v2);
}
} // namespace Spark

#endif