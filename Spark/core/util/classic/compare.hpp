#ifndef SPARK_COMPARE_HPP
#define SPARK_COMPARE_HPP

namespace Spark {
	template <typename T> struct Less {
		bool operator()(const T& a, const T& b) const { return a < b; }
	};

	template <typename T> struct Greater {
		bool operator()(const T& a, const T& b) const { return a > b; }
	};
	
}


#endif