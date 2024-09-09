#ifndef SPARK_CONST_PTR_HPP
#define SPARK_CONST_PTR_HPP

#include <core/pch.hpp>
#include "ref_ptr.hpp"

namespace Spark {
	// ConstPtr is just a const RefPtr
	template <typename T>
	using ConstPtr = const RefPtr<T>;
	
	template<typename To, typename From> ConstPtr<To> ConstCast(const ConstPtr<From>& from) { return ConstPtr<To>(static_cast<To*>(const_cast<From*>(from.Get()))); }
}



#endif