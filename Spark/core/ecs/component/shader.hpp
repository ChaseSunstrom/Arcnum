#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include "../../spark.hpp"
#include "../../util/wrap.hpp"

#include "../../logging/log.hpp"

namespace spark
{
	class shader_preprocessor
	{
	public:
		shader_preprocessor() = default;

		std::string preprocess_shader(const std::string& file_path);
	private:
		std::string process_file(const std::string& file_path);

		std::string extract_path(const std::string& line);
	private:
		std::unordered_set<std::string> m_included_files;
		std::string m_library_base_path = "Spark/shaders/";
	};

	class shader_manager
	{
	public:
		static shader_manager& get()
		{
			static shader_manager instance;
			return instance;
		}
		// Tuple to allow the user to get the concated paths of the vertex and
		// fragment shaders
		std::tuple<std::string, GLuint> load_shader(
			const std::pair<std::optional<std::string>, std::optional<std::string>>&
			paths_opt);

		bool create_and_link_program(GLuint vertex_shader, GLuint fragment_shader, GLuint& out_program);

		GLuint get_shader(const std::string& vertex_path,
						  const std::string& fragment_path);

		GLuint get_shader(const std::string& concat_paths);

		void delete_shader(const std::string& vertex_path,
						   const std::string& fragment_path);

		void delete_shader(const std::string& concat_paths);

	private:
		shader_manager() = default;

		~shader_manager();

		GLuint compile_shader(const std::string& shader_code, GLenum shader_type);

		inline std::string concat_paths(const std::string& vertex_path,
										const std::string& fragment_path);

	private:
		// Uses a concatenated string of the vertex and fragment shader paths
		std::unordered_map<std::string, GLuint> m_shaders =
			std::unordered_map<std::string, GLuint>();
		std::unique_ptr<shader_preprocessor> m_preprocessor =
			std::make_unique<shader_preprocessor>();
	};
} // namespace spark

#endif