#ifndef SPARK_COMPARE_HPP
#define SPARK_COMPARE_HPP

namespace Spark {
	template <typename _Ty> struct Less {
		bool operator()(const _Ty& a, const _Ty& b) const { return a < b; }
	};

	template <typename _Ty> struct Greater {
		bool operator()(const _Ty& a, const _Ty& b) const { return a > b; }
	};
	
}


#endif