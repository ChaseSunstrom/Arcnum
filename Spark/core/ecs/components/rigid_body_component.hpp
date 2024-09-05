#ifndef SPARK_RIGID_BODY_COMPONENT
#define SPARK_RIGID_BODY_COMPONENT

#include <core/ecs/component.hpp>
#include <core/physics/rigid_body.hpp>

namespace Spark {
	struct RigidBodyComponent : public Component {
		template<typename... Ts> RigidBodyComponent(Ts&&... args)
			: rb(std::make_unique<RigidBody>(std::forward<Ts>(args)...)) {}
		std::unique_ptr<RigidBody> rb;
	};
} // namespace Spark

#endif