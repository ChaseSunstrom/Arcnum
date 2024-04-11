#include "shader.hpp"

#include "../../util/file.hpp"
#include "../../util/wrap.hpp"
#include "../../window/window_manager.hpp"

namespace spark
{
	shader_type shader_wrapper::get_shader_type_from_extension(const std::string& file_extension)
	{
		if (file_extension == ".vert")
		{
			return shader_type::VERTEX;
		}
		else if (file_extension == ".frag")
		{
			return shader_type::FRAGMENT;
		}
		else if (file_extension == ".geom")
		{
			return shader_type::GEOMETRY;
		}
		else if (file_extension == ".comp")
		{
			return shader_type::COMPUTE;
		}
		else if (file_extension == ".tesc")
		{
			return shader_type::TESS_CONTROL;
		}
		else if (file_extension == ".tese")
		{
			return shader_type::TESSELATION_EVAL;
		}
		else
		{
			SPARK_ERROR("[SHADER] Shader type not supported");
			return shader_type::UNKNOWN;
		}
	}

	void shader_wrapper::create_vulkan_shader(const std::filesystem::path& shader_code)
	{
		auto& _vulkan_window = engine::get<vulkan_window>();

		std::string code = read_file(shader_code);
		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shader_module;
		if (vkCreateShaderModule(_vulkan_window.get_window_data().m_device, &create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			SPARK_ERROR("[SHADER] Failed to create shader module");
		}

		m_shader_variant = shader_module;
	}

	void shader::create_shader(const std::filesystem::path& shader_path)
	{
		std::string file_extension = get_file_extension(shader_path);
		shader_type type = m_shader->get_shader_type_from_extension(file_extension);

		m_shader->m_shader_type = type;

		switch (get_current_window_type())
		{
			case window_type::DIRECTX:
			{
				// Create DirectX shader
				break;
			}
			case window_type::VULKAN:
			{
				m_shader->create_vulkan_shader(shader_path);
				break;
			}
			case window_type::METAL:
			{
				// Create Metal shader
				break;
			}
		}
	}

	shader_manager::~shader_manager()
	{
		for (auto& it: m_shaders)
		{
			delete_program(it.second);
		}
	}

	shader shader_manager::load_shader(const std::filesystem::path& shader_path)
	{
		return shader(shader_path);
	}

	GLuint shader_manager::get_shader(const std::string& path)
	{
		return m_shaders[path];
	}
}