#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include "../../spark.hpp"
#include "../../util/wrap.hpp"

namespace spark
{
	class shader_manager
	{
	public:
		shader_manager() = default;

		~shader_manager();

		// Tuple to allow the user to get the concated paths of the vertex and fragment shaders
		std::tuple <std::string, GLuint> load_shader(const std::pair <std::optional<std::string>, std::optional<std::string>>& paths_opt);

		GLuint get_shader(const std::string& vertex_path, const std::string& fragment_path);

		GLuint get_shader(const std::string& concat_paths);

		void delete_shader(const std::string& vertex_path, const std::string& fragment_path);

		void delete_shader(const std::string& concat_paths);

	private:
		GLuint compile_shader(const std::string& shader_code, GLenum shader_type);

		inline std::string concat_paths(const std::string& vertex_path, const std::string& fragment_path);

	private:
		// Uses a concatenated string of the vertex and fragment shader paths
		std::unordered_map <std::string, GLuint> m_shaders = std::unordered_map<std::string, GLuint>();
	};
}

#endif