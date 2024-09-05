#ifndef SPARK_TRANSFORM_COMPONENT_HPP
#define SPARK_TRANSFORM_COMPONENT_HPP

#include <core/ecs/component.hpp>
#include <core/pch.hpp>
#include <core/scene/transform.hpp>

namespace Spark {
	struct TransformComponent : public Component {
		TransformComponent(const Transform& transform)
			: transform(transform) {}

		Transform transform;
	};
} // namespace Spark

#endif