#ifndef SPARK_INSTANCER_HPP
#define SPARK_INSTANCER_HPP

#include "../ecs/ecs.hpp"
#include "../scene/scene.hpp"
#include "../util/Singleton.hpp"

namespace Spark
{
struct Transforms
{
    Transforms() = default;

    void add_transform(const Transform &transform);

    void update();

    void update_render_transforms();

    std::vector<Transform> m_data;

    std::unordered_map<Entity, Transform> m_entity_transforms;
};

class Instancer : public Observer, public Singleton<Instancer>
{
  public:
    static Instancer &get()
    {
        static Instancer instance;
        return instance;
    }

    void add_renderable(Entity e, const Scene &scene, const std::string &mesh_name, const std::string &material_name,
                        const Transform &transform);

    void bind_renderables(const std::string &mesh_name,
                          const std::string &material_name);

    void render_instanced(Scene &scene);

    void remove_renderable_for_entity(Entity e);

    void update_renderable(Entity e, Scene &scene);

    void on_notify(std::shared_ptr<Event> event) override;

  private:
    Instancer() = default;

    ~Instancer() = default;

  private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<Transforms>>> m_renderables;

    std::unordered_map<Entity, std::pair<std::string, std::string>> m_entity_mesh_materials;
};
} // namespace Spark

#endif