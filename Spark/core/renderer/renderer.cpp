#include "../app/app.hpp"
#include "../ecs/component/shader.hpp"

namespace Spark
{
void Renderer::Transforms::add_transform(const Transform &transform)
{
    m_data.emplace_back(transform);
}

void Renderer::Transforms::update_render_transforms()
{
    m_data.clear();
    for (auto &[entity, transform] : m_entity_transforms)
    {
        m_data.emplace_back(transform);
    }
}

void Renderer::add_renderable(Entity e, const Scene &scene, const std::string &mesh_name,
                               const std::string &material_name, const Transform &_transform)
{
    auto &material_map = m_renderables[mesh_name];
    if (!material_map[material_name])
    {
        material_map[material_name] = std::make_unique<Transforms>();
    }
    material_map[material_name]->add_transform(_transform);
    m_entity_mesh_materials[e] = {mesh_name, material_name};
}

void Renderer::remove_renderable_for_entity(Entity e)
{
    auto [mesh_name, material_name] = m_entity_mesh_materials[e];

    auto &transform_storage = m_renderables[mesh_name][material_name];

    try
    {
        transform_storage->m_entity_transforms.erase(e);

        transform_storage->update_render_transforms();
    }
    catch (const std::exception &e)
    {
        SPARK_ERROR("[RENDERER] Error removing renderable for entity: " << e.what());
    }
}

void Renderer::on_notify(std::shared_ptr<Event> event)
{
    auto &ecs = Engine::get<ECS>();
    auto &scene = Engine::get<SceneManager>().get_current_scene();

    switch (event->m_type)
    {
    case ENTITY_CREATED_EVENT: {
        auto _entity_created_event = std::static_pointer_cast<EntityCreatedEvent>(event);
        Entity e = _entity_created_event->m_entity;

        if (ecs.has_component<TransformComponent>(e) && ecs.has_component<MeshComponent>(e) &&
            ecs.has_component<MaterialComponent>(e))
        {
            auto mesh_name = ecs.get_component<MeshComponent>(e).m_mesh_name;
            auto material_name = ecs.get_component<MaterialComponent>(e).m_material_name;
            auto &transform = ecs.get_component<TransformComponent>(e);

            add_renderable(e, scene, mesh_name, material_name, transform);
        }
        break;
    }
    case ENTITY_UPDATED_EVENT: {
        auto _entity_updated_event = std::static_pointer_cast<EntityUpdatedEvent>(event);
        Entity e = _entity_updated_event->m_entity;

        update_renderable(e, scene);
        break;
    }
    case ENTITY_DESTROYED_EVENT: {
        auto _entity_destroyed_event = std::static_pointer_cast<EntityDestroyedEvent>(event);
        Entity e = _entity_destroyed_event->m_entity;

        remove_renderable_for_entity(e);
        break;
    }
    }
}

void Renderer::update_renderable(Entity e, Scene &scene)
{
    // Check if the entity exists in the map
    if (m_entity_mesh_materials.find(e) != m_entity_mesh_materials.end())
    {
        // Get the mesh and material names associated with the entity
        const auto &[mesh_name, material_name] = m_entity_mesh_materials[e];

        // Check if the renderable object exists in the renderables map
        if (m_renderables.find(mesh_name) != m_renderables.end() &&
            m_renderables[mesh_name].find(material_name) != m_renderables[mesh_name].end())
        {
            // Update the transformation for the renderable object
            auto &transform_storage = m_renderables[mesh_name][material_name];
            if (transform_storage)
            {
                // Get the transform component from the ECS
                auto &transform = Engine::get<ECS>().get_component<TransformComponent>(e);

                // Update the transform in the storage
                transform_storage->m_entity_transforms[e] = transform;

                // Notify the transform storage to update its render transforms
                transform_storage->update_render_transforms();
            }
        }
    }
}
} // namespace Spark