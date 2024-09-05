#ifndef SPARK_QUERY_HPP
#define SPARK_QUERY_HPP

#include <core/pch.hpp>

namespace Spark {

	class Component;

	template<typename T>
	concept IsComponent                 = std::derived_from<T, Component>;

	template<IsComponent T> using Query = std::vector<T>;
} // namespace Spark

#endif