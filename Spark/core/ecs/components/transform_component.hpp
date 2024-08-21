#ifndef SPARK_TRANSFORM_COMPONENT_HPP
#define SPARK_TRANSFORM_COMPONENT_HPP

#include <core/pch.hpp>
#include <core/ecs/component.hpp>
#include <core/scene/transform.hpp>

namespace Spark
{
	struct TransformComponent : public Component
	{
		TransformComponent(const Transform& transform) : transform(transform) {}

		Transform transform;
	};
}

#endif