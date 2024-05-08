#ifndef SPARK_SCENE_PARTITION_HPP
#define SPARK_SCENE_PARTITOIN_HPP

#include "../ecs/ecs.hpp"
#include "../ecs/observer.hpp"

namespace Spark {
class SpatialPartition : public virtual Observer {
public:
  SpatialPartition(SpatialPartition *parent = nullptr) : Observer() {}
  virtual ~SpatialPartition() = default;

  virtual void add_entity(Entity e) {}
  virtual void remove_entity(Entity e) {}
  virtual void update_entity(Entity e) {}

  virtual void on_notify(std::shared_ptr<Event> event) {}

  virtual bool entity_is_inside(Entity e) const { return false; }

  virtual std::vector<Entity> query(const math::vec3 &position,
                                    f32 radius) const {
    return std::vector<Entity>();
  }
  virtual std::vector<Entity> query(const math::vec3 &position, f32 radius,
                                    std::function<bool(Entity)> filter) const {
    return std::vector<Entity>();
  }
  virtual std::vector<Entity> query(const math::vec3 &min,
                                    const math::vec3 &max) const {
    return std::vector<Entity>();
  }
  virtual std::vector<Entity> query(const math::vec3 &min,
                                    const math::vec3 &max,
                                    std::function<bool(Entity)> filter) const {
    return std::vector<Entity>();
  }

protected:
  std::vector<Entity> m_entities;
  SpatialPartition *m_parent;
};
} // namespace spark

#endif