#ifndef SPARK_GENERAL_HPP
#define SPARK_GENERAL_HPP

#include <core/util/container/pair.hpp>
#include <core/util/classic/util.hpp>

namespace Spark {
	template<typename V1, typename V2> Pair<V1, V2> PairOf(const V1& v1, const V2& v2) { return Pair<V1, V2>(v1, v2); }

	template<typename V1, typename V2> Pair<V1, V2> PairOf(V1&& v1, V2&& v2) { return Pair<V1, V2>(Move(v1), Move(V2)); } 
	
	template<typename V1, typename V2> Pair<V1, V2> PairOf(const V1& v1, V2&& v2) { return Pair<V1, V2>(v1, Move(V2)); } 

	template<typename V1, typename V2> Pair<V1, V2> PairOf(V1&& v1, const V2& v2) { return Pair<V1, V2>(Move(v1), V2); } 

	template<typename V2> Pair<const char*, V2> PairOf(const char* v1, const V2& v2) { return Pair<const char*, V2>(v1, v2); }
} // namespace Spark

#endif