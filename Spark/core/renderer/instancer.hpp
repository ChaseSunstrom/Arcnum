#ifndef SPARK_INSTANCER_HPP
#define SPARK_INSTANCER_HPP

#include "../ecs/ecs.hpp"
#include "../scene/scene.hpp"

namespace spark
{
	struct transforms
	{
		transforms() { generate_vertex_buffer(m_vbo); }

		void add_transform(math::mat4& transform);

		void update();
		
		GLuint m_vbo;
		std::vector<math::mat4> m_data;
	};

	class instancer : public observer
	{
	public:
		instancer() = default;
		~instancer() = default;

		void add_renderable(const scene& scene, const std::string& mesh_name, const std::string& material_name, transform_component& transform);
		void add_renderable(const scene& scene, const std::string& mesh_name, const std::string& material_name, std::vector<transform_component>& transforms);

		void bind_renderables(const std::vector<std::unique_ptr<camera>>& cameras, const std::string& mesh_name, const std::string& material_name);

		// Placeholder to take in cameras for now
		void render_instanced(const std::vector<std::unique_ptr<camera>>& cameras, scene& scene);

		void remove_renderable_for_entity(entity e);

		void on_notify(std::shared_ptr<event> event) override;
	private:
		// stores all current renderable entities:
		// mesh_name -> material_name -> mat_vbo
		// (transforms stores a vector of mat4 and the transform_components) -> mat4 (this is the underlying data structure of a transform_component)
		std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<transforms>>> m_renderables;
	};
}

#endif