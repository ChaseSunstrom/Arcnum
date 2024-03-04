#include "shader.hpp"

#include "../../util/file.hpp"
#include "../../util/wrap.hpp"

namespace spark
{
	shader_manager::~shader_manager()
	{
		for (auto& it: m_shaders)
		{
			delete_program(it.second);
		}
	}

	std::tuple <std::string, GLuint> shader_manager::load_shader(const std::pair <std::optional<std::string>, std::optional<std::string>>& paths_opt)
	{

		auto vertex_path_opt = paths_opt.first;
		auto fragment_path_opt = paths_opt.second;

		std::filesystem::path vertex_path = "";
		std::filesystem::path fragment_path = "";

		if (vertex_path_opt == std::nullopt || fragment_path_opt == std::nullopt)
		{
			vertex_path = std::filesystem::absolute("Spark/shaders/vertex_shader.vert");
			fragment_path = std::filesystem::absolute("Spark/shaders/fragment_shader.frag");
		}
		else
		{
			vertex_path = std::filesystem::absolute(vertex_path_opt.value());
			fragment_path = std::filesystem::absolute(fragment_path_opt.value());
		}

		std::string vertex_code = read_file(vertex_path);
		std::string fragment_code = read_file(fragment_path);

		GLuint vertex_shader = compile_shader(vertex_code, GL_VERTEX_SHADER);
		GLuint fragment_shader = compile_shader(fragment_code, GL_FRAGMENT_SHADER);

		check_gl_error("Shader compilation");

		GLuint shader_program = create_program();
		attach_shader(shader_program, vertex_shader);
		attach_shader(shader_program, fragment_shader);

		check_gl_error("Shader attaching");

		link_program(shader_program);

		spark::delete_shader(vertex_shader);
		spark::delete_shader(fragment_shader);

		std::string shader_name = concat_paths(vertex_path.string(), fragment_path.string());
		m_shaders[shader_name] = shader_program;

		std::tuple <std::string, GLuint> tuple(shader_name, shader_program);

		return tuple;
	}

	inline GLuint shader_manager::get_shader(const std::string& concat_paths)
	{
		return m_shaders[concat_paths];
	}

	inline GLuint shader_manager::get_shader(const std::string& vertex_path, const std::string& fragment_path)
	{
		std::string name = concat_paths(vertex_path, fragment_path);
		return m_shaders[name];
	}

	void shader_manager::delete_shader(const std::string& vertex_path, const std::string& fragment_path)
	{
		std::string name = concat_paths(vertex_path, fragment_path);
		auto it = m_shaders.find(name);

		if (it != m_shaders.end())
		{
			delete_program(it->second);
			m_shaders.erase(it);
		}
	}

	void shader_manager::delete_shader(const std::string& concat_paths)
	{
		auto it = m_shaders.find(concat_paths);

		if (it != m_shaders.end())
		{
			delete_program(it->second);
			m_shaders.erase(it);
		}
	}

	GLuint shader_manager::compile_shader(const std::string& shader_code, GLenum shader_type)
	{
		GLuint shader = create_shader(shader_type);
		shader_source(shader, shader_code);
		spark::compile_shader(shader);
		return shader;
	}

	inline std::string shader_manager::concat_paths(
			const std::string& vertex_path, const std::string& fragment_path)
	{
		return vertex_path + fragment_path;
	}
}