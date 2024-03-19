#include "shader.hpp"

#include "../../util/file.hpp"
#include "../../util/wrap.hpp"

namespace spark
{
	std::string shader_preprocessor::preprocess_shader(const std::string& file_path)
	{
		m_included_files.clear(); // Clear previously included files
		return process_file(file_path);
	}

	// In shader_preprocessor
	std::string shader_preprocessor::process_file(const std::string& file_path)
	{
		if (m_included_files.find(file_path) != m_included_files.end())
		{
			return ""; // Prevent circular include
		}

		std::ifstream file(file_path);
		std::stringstream processed_shader;

		if (!file.is_open())
		{
			SPARK_ERROR("[SHADER PREPROCESSOR]: Could not open shader file: " << file_path);
			return "";
		}

		m_included_files.insert(file_path); // Mark this file as included

		std::string line;
		std::filesystem::path file_path_obj = file_path;
		std::filesystem::path directory_path = file_path_obj.parent_path();

		while (std::getline(file, line))
		{
			if (line.find("#include") == 0)
			{
				std::string include_file_name = extract_path(line);
				std::filesystem::path full_include_path;
				if (line.find("<") != std::string::npos)
				{
					full_include_path = include_file_name; // Library include
				}
				else
				{
					full_include_path = directory_path / include_file_name; // Local include
				}
				processed_shader << process_file(full_include_path.string()); // Recursive call
			}
			else
			{
				processed_shader << line << '\n';
			}
		}

		return processed_shader.str();
	}


	std::string shader_preprocessor::extract_path(const std::string& line)
	{
		std::size_t start_pos = line.find_first_of("\"<");
		std::size_t end_pos = line.find_last_of("\">");

		if (start_pos != std::string::npos && end_pos != std::string::npos && start_pos < end_pos)
		{
			bool is_library_include = line[start_pos] == '<';
			std::string path = line.substr(start_pos + 1, end_pos - start_pos - 1);

			if (is_library_include)
			{
				return m_library_base_path + "/" + path;
			}
			else
			{
				return path;
			}
		}

		SPARK_ERROR("[SHADER PREPROCESSOR]: Error processing #include directive: " << line);
		return "";
	}

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

		std::string vertex_code = read_file(m_preprocessor->preprocess_shader(vertex_path.string()));
		std::string fragment_code = read_file(m_preprocessor->preprocess_shader(fragment_path.string()));

		GLuint vertex_shader = compile_shader(vertex_code, GL_VERTEX_SHADER);
		GLuint fragment_shader = compile_shader(fragment_code, GL_FRAGMENT_SHADER);

		GLuint shader_program = create_program();
		attach_shader(shader_program, vertex_shader);
		attach_shader(shader_program, fragment_shader);

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

	inline std::string shader_manager::concat_paths(const std::string& vertex_path, const std::string& fragment_path)
	{
		return vertex_path + fragment_path;
	}
}