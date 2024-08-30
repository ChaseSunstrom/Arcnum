#ifndef SPARK_MODEL_COMPONENT_HPP
#define SPARK_MODEL_COMPONENT_HPP

#include <core/ecs/component.hpp>
#include <core/pch.hpp>

namespace Spark {
struct ModelComponent : public Component {
	std::string model_name;
};
} // namespace Spark

#endif