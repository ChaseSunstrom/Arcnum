#ifndef SPARK_COMPONENT_TYPES_HPP
#define SPARK_COMPONENT_TYPES_HPP

#include "../../spark.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "../../window/window_type.hpp"
#include "../../window/vulkan/vulkan_mesh.hpp"

namespace spark
{
	enum class texture_type
	{
		TWO_D = 2, THREE_D = 3
	};

	struct transform
	{
		transform(
				const math::vec3& position = math::vec3(0.0f),
				const math::vec3& rotation = math::vec3(0.0f),
				const math::vec3& scale = math::vec3(1.0f))
		{
			math::mat4 mat = math::mat4(1.0f); // Start with identity matrix

			// Apply translation, rotation, and scaling
			mat = math::translate(mat, position);
			mat = math::rotate(mat, math::radians(rotation.z), math::vec3(0.0f, 0.0f, 1.0f));
			mat = math::rotate(mat, math::radians(rotation.y), math::vec3(0.0f, 1.0f, 0.0f));
			mat = math::rotate(mat, math::radians(rotation.x), math::vec3(1.0f, 0.0f, 0.0f));
			mat = math::scale(mat, scale);

			m_transform = mat;
		}

		void update_matrix(const math::vec3& position, const math::vec3& rotation, const math::vec3& scale)
		{
			m_transform = math::mat4(1.0f);  // Reset to identity matrix
			m_transform = math::translate(m_transform, position);
			m_transform = math::rotate(m_transform, math::radians(rotation.z), math::vec3(0.0f, 0.0f, 1.0f));
			m_transform = math::rotate(m_transform, math::radians(rotation.y), math::vec3(0.0f, 1.0f, 0.0f));
			m_transform = math::rotate(m_transform, math::radians(rotation.x), math::vec3(1.0f, 0.0f, 0.0f));
			m_transform = math::scale(m_transform, scale);
		}

		transform& operator*=(const transform& rhs)
		{
			this->m_transform = this->m_transform * rhs.m_transform;
			return *this;
		}

		transform operator*(const transform& rhs) const
		{
			transform result = *this; // Copy current transform
			result *= rhs;            // Apply multiplication
			return result;
		}

		math::mat4 m_transform;
	};

	class texture
	{
	public:
		texture() = default;

		texture(
				const std::filesystem::path& path,
				texture_type type,
				std::optional <i32> depth = std::nullopt,
				const std::vector <std::pair<GLenum, GLenum>>& params = std::vector < std::pair < GLenum,
				GLenum>>());

		~texture();

		void bind(GLenum texture_unit = GL_TEXTURE0);

		static texture& create_default_texture()
		{
			static bool created = false;

			static texture default_tex;

			if (created)
			{
				return default_tex;
			}

			created = true;

			std::vector <std::pair<GLenum, GLenum>> default_params = {
					{   GL_TEXTURE_MIN_FILTER, GL_LINEAR }
					, { GL_TEXTURE_MAG_FILTER, GL_LINEAR }
					, { GL_TEXTURE_WRAP_S    , GL_CLAMP_TO_EDGE }
					, { GL_TEXTURE_WRAP_T    , GL_CLAMP_TO_EDGE }
			};

			unsigned char white_pixel[3] = { 255, 255, 255 };
			default_tex.m_width = 1;
			default_tex.m_height = 1;
			default_tex.m_nr_channels = 3;
			default_tex.m_type = texture_type::TWO_D;
			default_tex.generate_texture(GL_TEXTURE_2D, GL_RGB, GL_RGB, GL_UNSIGNED_BYTE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, &white_pixel);
			default_tex.set_texture_parameters(GL_TEXTURE_2D, default_params);

			return default_tex;
		}

		void set_texture_parameters(GLenum target, const std::vector <std::pair<GLenum, GLenum>>& params);

		GLenum get_gl_texture_type() const;

	public:

		u32 m_texture;

		void* m_image_data = nullptr;

		i32 m_width;

		i32 m_height;

		i32 m_nr_channels;

		std::optional <i32> m_depth;

		texture_type m_type;

	private:

		void load_texture(const std::filesystem::path& path, const std::vector <std::pair<GLenum, GLenum>>& params);

		void generate_texture(GLenum target, GLenum internal_format, GLenum format, GLenum type);
	};

	struct material
	{
		material(
				const math::vec4& color, texture& tex, i32 diffuse, i32 specular, i32 ambient, f32 shininess) :
				m_color(color), m_diffuse(diffuse), m_specular(specular), m_ambient(ambient), m_shininess(shininess), m_texture(
				tex) { }

		~material() = default;

		math::vec4 m_color;

		i32 m_diffuse;

		i32 m_specular;

		i32 m_ambient;

		f32 m_shininess;

		texture& m_texture;
	};


	struct mesh_component
	{
		mesh_component() = default;

		mesh_component(const std::string& name) :
				m_mesh_name(name) { }

		~mesh_component() = default;

		std::string m_mesh_name = "";
	};

	struct material_component
	{
		material_component() = default;

		material_component(const std::string& name) :
				m_material_name(name) { }

		~material_component() = default;

		bool operator!=(const material_component& other) const
		{
			return m_material_name != other.m_material_name;
		}

		std::string m_material_name = "";
	};

	struct render_component
	{
		render_component() = default;

		render_component(bool has_mesh, bool has_transform, bool has_material) :
				m_renderable(has_mesh && has_transform && has_material) { }


		bool m_renderable = false;
	};

	class texture_manager
	{
	public:
		static texture_manager& get()
		{
			static texture_manager instance;
			return instance;
		}

		texture& create_texture(
				const std::string& name,
				const std::filesystem::path& path,
				texture_type type,
				std::optional <i32> depth = std::nullopt,
				const std::vector <std::pair<GLenum, GLenum>>& params = {
						{   GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR }
						, { GL_TEXTURE_MAG_FILTER, GL_LINEAR }
						, { GL_TEXTURE_WRAP_S    , GL_REPEAT }
						, { GL_TEXTURE_WRAP_T    , GL_REPEAT }
				})
		{
			m_textures[name] = std::make_unique<texture>(path, type, depth, params);
			return *m_textures[name];
		}

		texture& get_texture(const std::string& name);

		std::unordered_map <std::string, std::unique_ptr<texture>>& get_textures()
		{
			return m_textures;
		}

		texture& create_default_texture()
		{
			static std::string default_texture_name = "__default__";

			auto found = m_textures.find(default_texture_name);
			if (found != m_textures.end())
			{
				return *found->second;
			}

			// Create a new texture object
			texture default_tex;
			default_tex.m_type = texture_type::TWO_D;
			default_tex.m_width = 1;
			default_tex.m_height = 1;
			default_tex.m_nr_channels = 3;

			// Generate and bind the texture
			glGenTextures(1, &default_tex.m_texture);
			glBindTexture(GL_TEXTURE_2D, default_tex.m_texture);

			// Define the texture image (1x1 white pixel)
			uint8_t white_pixel[3] = { 255, 255, 255 };
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, white_pixel);

			std::vector <std::pair<GLenum, GLenum>> default_params = {
					{   GL_TEXTURE_MIN_FILTER, GL_LINEAR }
					, { GL_TEXTURE_MAG_FILTER, GL_LINEAR }
					, { GL_TEXTURE_WRAP_S    , GL_CLAMP_TO_EDGE }
					, { GL_TEXTURE_WRAP_T    , GL_CLAMP_TO_EDGE }
			};
			default_tex.set_texture_parameters(GL_TEXTURE_2D, default_params);

			glBindTexture(GL_TEXTURE_2D, 0);

			m_textures[default_texture_name] = std::make_unique<texture>(std::move(default_tex));

			return *m_textures[default_texture_name];
		}

		void destroy_texture(const std::string& name);

	private:
		texture_manager() = default;

		~texture_manager() = default;

	private:
		std::unordered_map <std::string, std::unique_ptr<texture>> m_textures;
	};

	class material_manager
	{
	public:
		static material_manager& get()
		{
			static material_manager instance;
			return instance;
		}

		material& create_material(
				const std::string& name,
				const std::pair <std::optional<std::string>, std::optional<std::string>>& shader_paths = {
						std::nullopt, std::nullopt
				},
				const math::vec4& color = math::vec4(1),
				const std::string& texture_name = "",
				i32 diffuse = 0,
				i32 specular = 0,
				i32 ambient = 1,
				f32 shininess = 0)
		{
			if (m_materials.contains(name))
			{
				return *m_materials[name];
			}

			texture* tex = nullptr;

			if (!texture_name.empty() && m_texture_manager.get_textures().contains(texture_name))
			{
				tex = &(m_texture_manager.get_texture(texture_name));
			}
			else
			{
				tex = &(m_texture_manager.create_default_texture());
			}

			m_materials[name] = std::make_unique<material>(
					color, *tex, diffuse, specular, ambient, shininess);

			return *m_materials[name];
		}

		void load_material(const std::string& name, std::unique_ptr <material> mat)
		{
			m_materials[name] = std::move(mat);
		}

		material& get_material(const std::string& name)
		{
			return *m_materials.at(name);
		}

		void destroy_material(const std::string& name)
		{
			m_materials.erase(name);
		}

		texture& create_texture(
				const std::string& name,
				const std::filesystem::path& path,
				texture_type type,
				std::optional <i32> depth = std::nullopt,
				const std::vector <std::pair<GLenum, GLenum>>& params = {
						{   GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR }
						, { GL_TEXTURE_MAG_FILTER, GL_LINEAR }
						, { GL_TEXTURE_WRAP_S    , GL_REPEAT }
						, { GL_TEXTURE_WRAP_T    , GL_REPEAT }
				})
		{
			return m_texture_manager.create_texture(name, path, type, depth, params);
		}

	private:
		material_manager()
		{
			create_material("__default__");
		}

		~material_manager() = default;

	private:
		shader_manager& m_shader_manager = shader_manager::get();

		texture_manager& m_texture_manager = texture_manager::get();

		std::unordered_map <std::string, std::unique_ptr<material>> m_materials;
	};

	class mesh_manager
	{
	public:
		static mesh_manager& get()
		{
			static mesh_manager instance;
			return instance;
		}

		template <typename... Args>
		mesh& create_mesh(
				const std::string& name,
				const std::vector <vertex>& vertices,
				const std::vector <u32> indices = { },
				Args&& ... args)
		{
			if (m_meshes.contains(name))
			{
				return *m_meshes[name];
			}

			if (get_current_window_type() == window_type::VULKAN)
			{
				m_meshes[name] = std::make_unique<vulkan_mesh>(vertices, indices, std::forward<Args>(args)...);
				return *m_meshes[name];
			}
		}

		void load_mesh(const std::string& name, std::unique_ptr <mesh> mesh)
		{
			m_meshes[name] = std::move(mesh);
		}

		mesh& get_mesh(const std::string& name)
		{
			return *m_meshes[name];
		}

		void destroy_mesh(const std::string& name)
		{
			m_meshes.erase(name);
		}

	private:
		mesh_manager() = default;

		~mesh_manager() = default;

	private:
		std::unordered_map <std::string, std::unique_ptr<mesh>> m_meshes;
	};

	// ==============================================================================
	// GLSL FUNCTIONS: 

	void set_uniform(const material& material, u32 shader_program);

	void set_uniform(const std::string& uniform_name, const material& material, u32 shader_program);

	// ==============================================================================

	// Types
	using transform_component = transform;
}

#endif