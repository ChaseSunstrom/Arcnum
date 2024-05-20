#ifndef SPARK_ECS_HPP
#define SPARK_ECS_HPP

#include "../audio/audio.hpp"
#include "../lighting/light.hpp"
#include "../net/serializeable.hpp"
#include "../spark.hpp"
#include "../util/singleton.hpp"
#include "component/component.hpp"
#include "component/component_types.hpp"
#include "component/mesh_manager.hpp"
#include "component/material_manager.hpp"
#include "component/texture_manager.hpp"
#include "entity/entity.hpp"
#include "observer.hpp"
#include "system/system.hpp"

namespace Spark
{
struct EntityCreatedEvent : public Event
{
    EntityCreatedEvent(Entity entity,
                       std::unordered_map<std::type_index, std::shared_ptr<IComponentInfo>> &&components_map)
        : Event(ENTITY_CREATED_EVENT), m_entity(entity)
    {
    }

    template <typename T> T get_component()
    {
        return static_cast<ComponentInfo<T>>(m_components[typeid(T)])->m_type;
    }

    Entity m_entity;

    std::unordered_map<std::type_index, std::shared_ptr<IComponentInfo>> m_components;
};

struct EntityUpdatedEvent : public Event
{
    EntityUpdatedEvent(Entity entity, std::unordered_map<std::type_index, std::shared_ptr<IComponentInfo>> components)
        : Event(ENTITY_UPDATED_EVENT), m_entity(entity), m_components(components)
    {
    }

    template <typename T> T get_component()
    {
        return static_cast<ComponentInfo<T>>(m_components[typeid(T)])->m_type;
    }

    Entity m_entity;

    std::unordered_map<std::type_index, std::shared_ptr<IComponentInfo>> m_components;
};

struct EntityDestroyedEvent : public Event
{
    EntityDestroyedEvent(Entity entity) : Event(ENTITY_DESTROYED_EVENT), m_entity(entity)
    {
    }

    Entity m_entity;
};

class ECS : public Singleton<ECS>
{
    friend class Singleton<ECS>;
  public:
    // ==============================================================================
    // Interface functions for all the managers:
    static ECS &get()
    {
        static ECS instance;
        return instance;
    }

    template <typename T> constexpr inline void register_component()
    {
        m_component_manager.register_component<T>();
    }

    template <typename... components> constexpr inline void register_components()
    {
        m_component_manager.register_components<components...>();
    }

    template <typename T> inline void add_component(Entity entity, const T &component)
    {
        m_component_manager.add_component<T>(entity, component);

        std::shared_ptr<EntityUpdatedEvent> event = std::make_shared<EntityUpdatedEvent>(
            entity, std::unordered_map<std::type_index, std::shared_ptr<IComponentInfo>>{
                        {typeid(T), std::make_shared<ComponentInfo<T>>(component)}});

        notify_observers(event);
    }

    template <typename T> inline void set_component(Entity entity, const T &component)
    {
        m_component_manager.set_component<T>(entity, component);
    }

    template <typename T> inline T &get_component(Entity entity)
    {
        return m_component_manager.get_component<T>(entity);
    }

    template <typename T> inline ComponentArray<T> &get_component_array()
    {
        return m_component_manager.get_component_array<T>();
    }

    template <typename T> inline bool has_component(Entity entity)
    {
        return m_component_manager.has_component<T>(entity);
    }

    inline std::vector<IComponentInfo> get_all_components(Entity entity)
    {
        return m_component_manager.get_all_components(entity);
    }

    template <typename T, typename... Args> inline T &register_update_system(Args &&...args)
    {
        return m_system_manager.register_update_system<T>(std::forward<Args>(args)...);
    }

    template <typename T, typename... Args> inline T &register_start_system(Args &&...systems)
    {
        return m_system_manager.register_start_system<T>(std::forward<Args>(systems)...);
    }

    template <typename T, typename... Args> inline T &register_shutdown_system(Args &&...systems)
    {
        return m_system_manager.register_shutdown_system<T>(std::forward<Args>(systems)...);
    }

    inline void start_systems()
    {
        m_system_manager.start_systems();
    }

    inline void update_systems(f64 delta_time)
    {
        m_system_manager.update_systems(delta_time);
    }

    inline void shutdown_systems()
    {
        m_system_manager.shutdown_systems();
    }

    inline Entity create_entity()
    {
        return m_entity_manager.create_entity();
    }

    inline bool entity_is_renderable(Entity entity)
    {
        return has_component<MeshComponent>(entity) && has_component<TransformComponent>(entity) &&
               has_component<MaterialComponent>(entity);
    }

    inline void destroy_entity(Entity entity)
    {
        m_entity_manager.destroy_entity(entity);
        m_component_manager.destroy_component_array(entity);

        std::shared_ptr<EntityDestroyedEvent> event = std::make_shared<EntityDestroyedEvent>(entity);

        notify_observers(event);
    }

    void add_observer(Observer *observer)
    {
        m_observers.push_back(std::move(observer));
    }

    void remove_observer(Observer &_observer)
    {
        m_observers.erase(std::remove_if(m_observers.begin(), m_observers.end(),
                                         [&_observer](Observer *o) { return o == &_observer; }),
                          m_observers.end());
    }

    template <typename... Components> Entity create_entity(const Components &...components)
    {
        Entity new_entity = m_entity_manager.create_entity();

        std::unordered_map<std::type_index, std::shared_ptr<IComponentInfo>> components_map;

        auto add_component = [this, &components_map, &new_entity](const auto &component) {
            using component_type = std::decay_t<decltype(component)>;
            m_component_manager.add_component<component_type>(new_entity, component);

            components_map[typeid(component_type)] = std::make_shared<ComponentInfo<component_type>>(component);
        };

        (add_component(components), ...);

        std::shared_ptr<EntityCreatedEvent> event =
            std::make_shared<EntityCreatedEvent>(new_entity, std::move(components_map));

        notify_observers(event);

        return new_entity;
    }

    // ==============================================================================
  private:
    ECS()
    {
        // Registers Engine specific components on initialization of the ECS
        // custom cumponents will need to be registered manually
        register_components<
            // Rendering
            MaterialComponent, TransformComponent, MeshComponent,
            // Audio
            AudioComponent,
            // Lighting
            DirectionalLightComponent, PointLightComponent, SpotLightComponent>();
        
        register_update_system<AudioSystem>(&get_component_array<AudioComponent>().get_array());
    }

    ~ECS() = default;

    void notify_observers(std::shared_ptr<Event> event)
    {
        for (auto &observer : m_observers)
        {
            observer->on_notify(event);
        }
    }

  private:
    std::vector<Observer *> m_observers;

    ComponentManager &m_component_manager = ComponentManager::get();

    EntityManager &m_entity_manager = EntityManager::get();

    SystemManager &m_system_manager = SystemManager::get();

    SERIALIZE_MEMBERS(ECS, m_component_manager, m_entity_manager, m_system_manager)
};
} // namespace Spark

#endif // CORE_ECS_H