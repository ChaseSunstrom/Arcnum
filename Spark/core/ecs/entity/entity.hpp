#ifndef SPARK_ENTITY_HPP
#define SPARK_ENTITY_HPP

#include "../../net/serializeable.hpp"
#include "../../spark.hpp"
#include "../component/component.hpp"
#include "../../util/Singleton.hpp"
#include "entity_type.hpp"

namespace Spark
{
class EntityManager : public Singleton<EntityManager>
{
    friend class Singleton<EntityManager>;
  public:

    Entity create_entity()
    {
        if (m_recycled_ids.empty())
        {
            return m_next_id++;
        }

        u64 id = m_recycled_ids.front();
        m_recycled_ids.pop_front();
        return id;
    }

    u64 get_entity_count() const
    {
        return m_next_id;
    }

    void destroy_entity(Entity e)
    {
        m_recycled_ids.push_front(e);
    }

  private:
    EntityManager() = default;

    ~EntityManager() = default;

  private:
    Entity m_next_id = 0;

    std::deque<Entity> m_recycled_ids;

    SERIALIZE_MEMBERS(EntityManager, m_next_id, m_recycled_ids)
};
} // namespace Spark

#endif // CORE_ENTITY_H