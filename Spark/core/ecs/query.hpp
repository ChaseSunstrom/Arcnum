#ifndef SPARK_QUERY_HPP
#define SPARK_QUERY_HPP

#include <core/pch.hpp>

namespace Spark {

	class Component;

	template<typename _Ty>
	concept IsComponent                 = std::derived_from<_Ty, Component>;

	template<IsComponent _Ty> using Query = Vector<_Ty>;
} // namespace Spark

#endif