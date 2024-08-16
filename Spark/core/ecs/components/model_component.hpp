#ifndef SPARK_MODEL_COMPONENT_HPP
#define SPARK_MODEL_COMPONENT_HPP

#include <core/pch.hpp>
#include <core/ecs/component.hpp>

namespace Spark
{
	struct ModelComponent : public Component
	{
		std::string model_name;
	};
}

#endif