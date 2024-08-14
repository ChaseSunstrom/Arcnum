#ifndef SPARK_QUERY_HPP
#define SPARK_QUERY_HPP

#include <core/pch.hpp>

namespace Spark
{

	class IComponent;

	template <typename T>
	concept IsComponent = std::derived_from<T, IComponent>;

	template <IsComponent T>
	using Query = std::vector<std::shared_ptr<T>>;
}

#endif