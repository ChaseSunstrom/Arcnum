#ifndef SPARK_OCTREE_HPP
#define SPARK_OCTREE_HPP

#include "../util/math.hpp"
#include "scene_partition.hpp"

namespace Spark
{
class Octree : public SpatialPartition
{
  public:
    Octree(const math::vec3 &center, const math::vec3 &half_size, Octree *parent = nullptr)
        : Observer(true), SpatialPartition(), m_center(center), m_half_size(half_size), m_parent(parent)
    {
        if (!parent)
            register_observer();

        for (i32 i = 0; i < 8; i++)
        {
            m_children[i] = nullptr;
        }
    }

    ~Octree() = default;

    void add_entity(Entity e) override
    {
        auto &ecs = Engine::get<ECS>();
        math::vec3 position = extract_position(ecs.get_component<Transform>(e).m_transform);

        if (!point_is_inside(position))
        {
            if (m_parent == nullptr)
            {                                         // Only the root can expand
                expand(position, m_half_size * 2.0f); // Double the size
            }
            else
            {
                return; // Cannot add entity, outside of bounds
            }
        }

        if (is_leaf())
        {
            m_entities.push_back(e);
            if (m_entities.size() > 8)
            { // Arbitrary threshold to subdivide
                subdivide();
            }
        }
        else
        {
            i32 octant = get_octant(position);
            m_children[octant]->add_entity(e);
        }
    }

    void remove_entity(Entity e)
    {
        auto &ecs = Engine::get<ECS>();
        math::vec3 position = extract_position(ecs.get_component<Transform>(e).m_transform);

        if (!point_is_inside(position))
        {
            return; // Entity not in this octree
        }

        if (is_leaf())
        {
            auto it = std::find(m_entities.begin(), m_entities.end(), e);
            if (it != m_entities.end())
            {
                m_entities.erase(it);
            }
        }
        else
        {
            i32 octant = get_octant(position);
            m_children[octant]->remove_entity(e);
        }
    }

    void update_entity(Entity e) override
    {
        remove_entity(e);
        add_entity(e);
    }

    bool point_is_inside(const math::vec3 &point) const
    {
        return (point.x >= m_center.x - m_half_size.x && point.x <= m_center.x + m_half_size.x) &&
               (point.y >= m_center.y - m_half_size.y && point.y <= m_center.y + m_half_size.y) &&
               (point.z >= m_center.z - m_half_size.z && point.z <= m_center.z + m_half_size.z);
    }

    bool entity_is_inside(Entity e) const override
    {
        auto &ecs = Engine::get<ECS>();
        math::vec3 position = extract_position(ecs.get_component<Transform>(e).m_transform);
        return point_is_inside(position);
    }

    bool is_leaf() const
    {
        return m_children[0] == nullptr;
    }

    bool intersects(const math::vec3 &center, const math::vec3 &half_size, const math::vec3 &position, f32 radius) const
    {
        // Check for intersection between a cube and a sphere
        f32 dmin = 0;
        for (int i = 0; i < 3; i++)
        {
            if (position[i] < center[i] - half_size[i])
            {
                dmin += (position[i] - (center[i] - half_size[i])) * (position[i] - (center[i] - half_size[i]));
            }
            else if (position[i] > center[i] + half_size[i])
            {
                dmin += (position[i] - (center[i] + half_size[i])) * (position[i] - (center[i] + half_size[i]));
            }
        }
        return dmin <= radius * radius;
    }

    bool intersects(const math::vec3 &center, const math::vec3 &half_size, const math::vec3 &a_min,
                    const math::vec3 &a_max) const
    {
        // Get min and max points of the octree node
        math::vec3 b_min = center - half_size;
        math::vec3 b_max = center + half_size;

        // Check for overlap in each dimension
        return (a_min.x <= b_max.x && a_max.x >= b_min.x) && (a_min.y <= b_max.y && a_max.y >= b_min.y) &&
               (a_min.z <= b_max.z && a_max.z >= b_min.z);
    }

    std::vector<Entity> query(const math::vec3 &query_position, f32 radius) const override
    {
        std::vector<Entity> result;
        if (intersects(m_center, m_half_size, query_position, radius))
        {
            if (is_leaf())
            {
                std::copy_if(m_entities.begin(), m_entities.end(), std::back_inserter(result), [&](const Entity &e) {
                    math::vec3 entity_pos =
                        extract_position(Engine::get<ECS>().get_component<Transform>(e).m_transform);
                    math::vec3 diff = entity_pos - query_position;
                    return math::dot(diff, diff) <= radius * radius;
                });
            }
            else
            {
                for (auto &child : m_children)
                {
                    auto child_results = child->query(query_position, radius);
                    result.insert(result.end(), child_results.begin(), child_results.end());
                }
            }
        }
        return result;
    }

    std::vector<Entity> query(const math::vec3 &query_position, f32 radius,
                              std::function<bool(Entity)> filter) const override
    {
        std::vector<Entity> result;
        if (intersects(m_center, m_half_size, query_position, radius))
        {
            if (is_leaf())
            {
                std::copy_if(m_entities.begin(), m_entities.end(), std::back_inserter(result), [&](const Entity &e) {
                    if (!filter(e))
                        return false;
                    math::vec3 entity_pos =
                        extract_position(Engine::get<ECS>().get_component<Transform>(e).m_transform);
                    math::vec3 diff = entity_pos - query_position;
                    return math::dot(diff, diff) <= radius * radius;
                });
            }
            else
            {
                for (auto &child : m_children)
                {
                    auto child_results = child->query(query_position, radius, filter);
                    result.insert(result.end(), child_results.begin(), child_results.end());
                }
            }
        }
        return result;
    }

    std::vector<Entity> query(const math::vec3 &min, const math::vec3 &max) const override
    {
        std::vector<Entity> result;
        if (intersects(m_center, m_half_size, min, max))
        {
            if (is_leaf())
            {
                std::copy_if(m_entities.begin(), m_entities.end(), std::back_inserter(result), [&](const Entity &e) {
                    math::vec3 entity_pos =
                        extract_position(Engine::get<ECS>().get_component<Transform>(e).m_transform);
                    return (entity_pos.x >= min.x && entity_pos.x <= max.x) &&
                           (entity_pos.y >= min.y && entity_pos.y <= max.y) &&
                           (entity_pos.z >= min.z && entity_pos.z <= max.z);
                });
            }
            else
            {
                for (auto &child : m_children)
                {
                    auto child_results = child->query(min, max);
                    result.insert(result.end(), child_results.begin(), child_results.end());
                }
            }
        }
        return result;
    }

    std::vector<Entity> query(const math::vec3 &min, const math::vec3 &max,
                              std::function<bool(Entity)> filter) const override
    {
        std::vector<Entity> result;
        if (intersects(m_center, m_half_size, min, max))
        {
            if (is_leaf())
            {
                std::copy_if(m_entities.begin(), m_entities.end(), std::back_inserter(result), [&](const Entity &e) {
                    if (!filter(e))
                        return false;
                    math::vec3 entity_pos =
                        extract_position(Engine::get<ECS>().get_component<Transform>(e).m_transform);
                    return (entity_pos.x >= min.x && entity_pos.x <= max.x) &&
                           (entity_pos.y >= min.y && entity_pos.y <= max.y) &&
                           (entity_pos.z >= min.z && entity_pos.z <= max.z);
                });
            }
            else
            {
                for (auto &child : m_children)
                {
                    auto child_results = child->query(min, max, filter);
                    result.insert(result.end(), child_results.begin(), child_results.end());
                }
            }
        }
        return result;
    }

    void on_notify(std::shared_ptr<Event> event) override
    {
        switch (event->m_type)
        {
        case ENTITY_CREATED_EVENT: {
            auto e = std::static_pointer_cast<EntityCreatedEvent>(event);
            add_entity(e->m_entity);
            break;
        }
        case ENTITY_UPDATED_EVENT: {
            auto e = std::static_pointer_cast<EntityUpdatedEvent>(event);
            update_entity(e->m_entity);
            break;
        }
        case ENTITY_DESTROYED_EVENT: {
            auto e = std::static_pointer_cast<EntityDestroyedEvent>(event);
            remove_entity(e->m_entity);
            break;
        }
        }
    }

    void expand(const math::vec3 &new_center, const math::vec3 &new_half_size)
    {
        if (m_parent != nullptr)
            return; // Ensure only the root can expand

        // Store old entities temporarily
        std::vector<Entity> old_entities;
        if (is_leaf())
        {
            old_entities.swap(m_entities); // Take entities from root if it's a leaf
        }
        else
        {
            // Collect entities from all children if not a leaf
            collect_entities(old_entities);
            for (auto &child : m_children)
            {
                child.reset(); // Clear out old children
            }
        }

        // Adjust size and reposition center
        m_center = new_center;
        m_half_size = new_half_size;

        // Reinsert entities according to the new boundaries
        for (const Entity &e : old_entities)
        {
            add_entity(e);
        }
    }

  private:
    void subdivide()
    {
        if (!is_leaf())
            return; // Already subdivided

        auto &ecs = Engine::get<ECS>();

        const math::vec3 child_half_size = m_half_size * 0.5f;
        for (int i = 0; i < 8; ++i)
        {
            math::vec3 child_center = m_center;
            if (i & 4)
                child_center.x += child_half_size.x;
            else
                child_center.x -= child_half_size.x;

            if (i & 2)
                child_center.y += child_half_size.y;
            else
                child_center.y -= child_half_size.y;

            if (i & 1)
                child_center.z += child_half_size.z;
            else
                child_center.z -= child_half_size.z;

            m_children[i] = std::make_unique<Octree>(child_center, child_half_size, this);
        }

        // Move entities to appropriate children
        for (const auto &e : m_entities)
        {
            auto position = extract_position(ecs.get_component<TransformComponent>(e).m_transform);
            i32 octant = get_octant(position);
            m_children[octant]->add_entity(e);
        }
        m_entities.clear(); // Clear entities after redistribution
    }

    void merge()
    {
        if (is_leaf())
            return; // No children to merge

        for (auto &child : m_children)
        {
            if (!child->is_leaf())
                return; // Ensure all children are leaves before merging

            // Transfer all entities from children to this node
            m_entities.insert(m_entities.end(), child->m_entities.begin(), child->m_entities.end());
            child.reset();
        }
    }

    i32 get_octant(const math::vec3 &position) const
    {
        i32 octant = 0;
        if (position.x >= m_center.x)
            octant |= 4;
        if (position.y >= m_center.y)
            octant |= 2;
        if (position.z >= m_center.z)
            octant |= 1;
        return octant;
    }

    void collect_entities(std::vector<Entity> &all_entities)
    {
        if (is_leaf())
        {
            all_entities.insert(all_entities.end(), m_entities.begin(), m_entities.end());
        }
        else
        {
            for (auto &child : m_children)
            {
                if (child)
                {
                    child->collect_entities(all_entities);
                }
            }
        }
    }

    math::vec3 m_center;
    math::vec3 m_half_size;
    Octree *m_parent;
    std::array<std::unique_ptr<Octree>, 8> m_children;
};
} // namespace Spark

#endif