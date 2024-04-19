#ifndef SPARK_RIGID_BODY_HPP
#define SPARK_RIGID_BODY_HPP

#include "../ecs/ecs.hpp"
#include "aabb.hpp"

namespace spark
{
	struct rigid_body
	{
		rigid_body(f64 mass = 1, transform* transform = nullptr, math::vec3 velocity = math::vec3(0.0f), std::unique_ptr<aabb> aabb = nullptr)
			: m_mass(mass), m_transform(transform), m_velocity(velocity), m_aabb(std::move(aabb))
		{}

		math::vec3 m_velocity;
		std::unique_ptr<aabb> m_aabb;
		f64 m_mass;
		transform* m_transform;
	};

	using rigid_body_component = rigid_body;
}

#endif