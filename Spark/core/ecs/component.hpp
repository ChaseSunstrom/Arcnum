#ifndef SPARK_COMPONENT_HPP
#define SPARK_COMPONENT_HPP

#include <core/pch.hpp>
#include "entity.hpp"

namespace Spark
{
	class IComponent 
	{
	public:
		virtual ~IComponent() = default;
	};
}

#endif