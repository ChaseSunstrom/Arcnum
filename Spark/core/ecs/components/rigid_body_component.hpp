#ifndef SPARK_RIGID_BODY_COMPONENT
#define SPARK_RIGID_BODY_COMPONENT

#include <core/pch.hpp>
#include <core/ecs/component.hpp>
#include <core/physics/rigid_body.hpp>

namespace Spark {
	struct RigidBodyComponent : public Component {
		template<typename... Ts> RigidBodyComponent(Ts&&... args)
			: rb(MakeUnique<RigidBody>(Forward<Ts>(args)...)) {}
		UniquePtr<RigidBody> rb;
	};
} // namespace Spark

#endif