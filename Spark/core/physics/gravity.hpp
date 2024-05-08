#ifndef SPARK_GRAVITY_HPP
#define SPARK_GRAVITY_HPP

#include "../spark.hpp"

#include "../ecs/ecs.hpp"
#include "rigid_body.hpp"

namespace Spark {
class GravitySystem : public System {
public:
  GravitySystem(f64 gravity = 0, f64 terminal_velocity = 1)
      : m_gravity(gravity), m_terminal_velocity(terminal_velocity) {}

  void on_init() override {}

  void on_start() override {}

  void on_update(f64 delta_time) override {
    auto &_ecs = Engine::get<ECS>();
    auto &_rigid_body_array = _ecs.get_component_array<rigid_body_component>();

    // Iterate over all entities with rigid body components
    for (auto &body_opt : _rigid_body_array.get_array()) {
      if (!body_opt.has_value())
        continue;

      rigid_body_component &body = body_opt.value();

      if (!body.m_transform)
        continue; // Skip if no transform is associated

      // Calculate the gravity force as an acceleration vector
      math::vec3 gravity_force(0, -m_gravity,
                               0); // Gravity only affects the y-component

      // Update velocity with gravity force applied over the given time delta
      body.m_velocity += gravity_force * (f32)delta_time;

      // Check for terminal velocity
      if (body.m_velocity.y < -m_terminal_velocity) {
        body.m_velocity.y = -m_terminal_velocity;
      }

      body.m_transform->m_transform += body.m_velocity * (f32)delta_time;
    }
  }

  void on_shutdown() override {}

private:
  f64 m_gravity;
  f64 m_terminal_velocity;
};
} // namespace spark

#endif