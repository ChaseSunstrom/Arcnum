#ifndef SPARK_INSTANCER_HPP
#define SPARK_INSTANCER_HPP

#include "../ecs/ecs.hpp"

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

	class instancer
	{
	public:
		instancer() = default;
		~instancer() = default;

		void add_renderable(const std::string& mesh_name, const std::string& material_name, math::mat4& transform);
		void add_renderable(const std::string& mesh_name, const std::string& material_name, std::vector<math::mat4>& transforms);

		void bind_renderables(const std::string& mesh_name, const std::string& material_name);

		void render_instanced();
	private:
		// stores all current renderable entities:
		// mesh_name -> material_name -> mat_vbo
		// (transforms stores a vector of mat4 and the transform_components) -> mat4 (this is the underlying data structure of a transform_component)
		std::unordered_map<std::string, std::unordered_map<std::string, std::unique_ptr<transforms>>> m_renderables;
	};
}

#endif