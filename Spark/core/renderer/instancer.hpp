#ifndef SPARK_INSTANCER_HPP
#define SPARK_INSTANCER_HPP

#include "../ecs/ecs.hpp"
#include "../scene/scene.hpp"

namespace spark
{
	struct transforms
	{
        transforms() = default;

		void add_transform(const transform& transform);

		void update();

        void update_render_transforms();
		
		std::vector<transform> m_data;
        std::unordered_map<entity, transform> m_entity_transforms;
	};

    class instancer : public observer
    {
    public:
        static instancer& get()
        {
            static instancer instance;
            return instance;
        }
        void add_renderable(entity e, const scene& scene, const std::string& mesh_name, const std::string& material_name, const transform& transform);

        void bind_renderables(const std::vector<std::unique_ptr<camera>>& cameras, const std::string& mesh_name, const std::string& material_name);

        void render_instanced(const std::vector<std::unique_ptr<camera>>& cameras, scene& scene);

        void remove_renderable_for_entity(entity e);

        void update_renderable(entity e, scene& scene);

        void on_notify(std::shared_ptr<event> event) override;
    private:

        instancer() = default;
        ~instancer() = default;

    private:
            std::unordered_map<std::string, 
                std::unordered_map<std::string, 
                    std::unique_ptr<transforms>>> m_renderables;

            std::unordered_map<entity, 
                std::pair<std::string, std::string>> m_entity_mesh_materials;
    };
}

#endif