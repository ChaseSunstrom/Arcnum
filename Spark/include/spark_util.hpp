#ifndef SPARK_UTIL_HPP
#define SPARK_UTIL_HPP

#include "spark_pch.hpp"

namespace spark
{
	template <auto Start, auto End, auto Inc, class F>
	constexpr void ConstexprFor(F&& f)
	{
		if constexpr (Start < End)
		{
			f(std::integral_constant<decltype(Start), Start>());
			ConstexprFor<Start + Inc, End, Inc>(f);
		}
	}

	template <typename F, typename... Args>
	constexpr void ConstexprFor(F&& f, Args&&... args)
	{
		(f(std::forward<Args>(args)), ...);
	}
}

#endif