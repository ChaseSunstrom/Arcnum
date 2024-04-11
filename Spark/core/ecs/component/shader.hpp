#ifndef SPARK_SHADER_HPP
#define SPARK_SHADER_HPP

#include "../../spark.hpp"
#include "../../util/wrap.hpp"

#include "../../logging/log.hpp"

namespace spark
{
	enum class shader_type
	{
		UNKNOWN = 0,
		VERTEX,
		FRAGMENT,
		GEOMETRY,
		COMPUTE,
		TESS_CONTROL,
		TESSELATION_EVAL
	};

	struct shader_wrapper
	{
		shader_wrapper() = default;
		shader_type get_shader_type_from_extension(const std::string& file_extension);

		// Only vulkan is implemented currently
		void create_vulkan_shader(const std::filesystem::path& shader_path);
		void create_metal_shader();
		void create_directx_shader();

		shader_type m_shader_type;
		std::variant<VkShaderModule> m_shader_variant;
	};

	class shader
	{
	public:
		shader(const std::filesystem::path& shader_path) {
			create_shader(shader_path); 
		}
		shader_type get_shader_type() const {
			return m_shader->m_shader_type; 
		}
	private:
		void create_shader(const std::filesystem::path& shader_path);
	private:
		std::unique_ptr<shader_wrapper> m_shader = std::make_unique<shader_wrapper>();
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
		shader load_shader(const std::filesystem::path& shader_path);

		GLuint get_shader(const std::string& path);
	private:
		shader_manager() = default;

		~shader_manager();

	private:
		// Uses a concatenated string of the vertex and fragment shader paths
		std::unordered_map<std::string, GLuint> m_shaders =
			std::unordered_map<std::string, GLuint>();
	};
} // namespace spark

#endif