#ifndef SPARK_AABB_HPP
#define SPARK_AABB_HPP

#include "../ecs/component/component_types.hpp"
#include "../spark.hpp"

namespace Spark
{
class AABB
{
  public:
    AABB(Math::vec3 min, Math::vec3 max) : m_min(min), m_max(max)
    {
    }

    AABB(const std::vector<Vertex> &vertices);

    bool check_collision(const AABB &other) const;

  private:
    Math::vec3 m_min;

    Math::vec3 m_max;
};
} // namespace Spark

#endif