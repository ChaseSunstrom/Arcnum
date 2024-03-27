#ifndef SPARK_COMPONENT_TYPES_HPP
#define SPARK_COMPONENT_TYPES_HPP

#include "../../spark.hpp"

#include "shader.hpp"

namespace spark
{
	struct vertex
	{
		vertex(const math::vec3& position = math::vec3(0), const math::vec3& normal = math::vec3(0), const math::vec2& texcoord = math::vec2(0)) :
			m_position(position), m_normal(normal), m_texcoords(texcoord)
		{}

		~vertex() = default;

		math::vec3 m_position;

		math::vec3 m_normal;

		math::vec2 m_texcoords;
	};

	enum class texture_type
	{
		TWO_D = 2, THREE_D = 3
	};

	struct transform
	{
		transform(
			const math::vec3& position = math::vec3(0.0f),
			const math::vec3& rotation = math::vec3(1.0f),
			const math::vec3& scale = math::vec3(1.0f)
		)
		{
			math::mat4 mat = math::mat4(1.0f); // Start with identity matrix

			// Apply translation
			mat = math::translate(mat, position);

			// Apply rotations around the Z, Y, and X axes, in that order
			mat = math::rotate(mat, math::radians(rotation.z), math::vec3(0.0f, 0.0f, 1.0f));
			mat = math::rotate(mat, math::radians(rotation.y), math::vec3(0.0f, 1.0f, 0.0f));
			mat = math::rotate(mat, math::radians(rotation.x), math::vec3(1.0f, 0.0f, 0.0f));

			// Apply scaling
			mat = math::scale(mat, scale);
			m_transform = mat;
		}

		bool operator!=(const transform& other) const
		{
			return m_transform != other.m_transform;
		}

		math::mat4 m_transform;
	};

	class texture
	{
	public:
		texture() = default;

		texture(const std::filesystem::path& path,
				texture_type type,
				std::optional <int32_t> depth = std::nullopt,
				const std::vector <std::pair<GLenum, GLenum>>& params = std::vector<std::pair < GLenum, GLenum>>());

		~texture();

		void bind(GLenum texture_unit = GL_TEXTURE0);

		static texture& create_default_texture()
		{
			static bool created = false;

			static texture default_tex;

			if (created)
				return default_tex;

			created = true;

			std::vector<std::pair<GLenum, GLenum>> default_params = {
				{GL_TEXTURE_MIN_FILTER, GL_LINEAR},
				{GL_TEXTURE_MAG_FILTER, GL_LINEAR},
				{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
				{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE}
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

		void set_texture_parameters(GLenum target, const std::vector<std::pair<GLenum, GLenum>>& params);

		GLenum get_gl_texture_type() const;
	public:

		uint32_t m_texture;

		void* m_image_data = nullptr;

		int32_t m_width;

		int32_t m_height;

		int32_t m_nr_channels;

		std::optional <int32_t> m_depth;

		texture_type m_type;

	private:

		void load_texture(const std::filesystem::path& path, const std::vector<std::pair<GLenum, GLenum>>& params);

		void generate_texture(GLenum target, GLenum internal_format, GLenum format, GLenum type);
	};

	struct mesh
	{
		mesh();

		mesh(const std::vector <vertex>& vertices);

		~mesh();

		mesh(mesh&& other) noexcept :
			m_vertices(std::move(other.m_vertices)),
			m_vao(other.m_vao)
		{
			// Transfer ownership
			other.m_vao = 0;
		}

		void bind();

		void unbind();

		void update(const std::vector <vertex>& vertices, const std::vector<GLuint>& indices);

		void create_mesh();

		GLuint get_vao()
		{
			return m_vao;
		}

		std::vector <vertex> m_vertices = std::vector<vertex>();
	private:
		GLuint m_vao = 0;
		GLuint m_vbo = 0;
	};

	struct material
	{
		material(
			GLuint shader_program,
			const math::vec4& color,
			texture& tex,
			int32_t diffuse,
			int32_t specular,
			int32_t ambient,
			float32_t shininess)
			: m_color(color), m_diffuse(diffuse), m_specular(specular),
			m_ambient(ambient), m_shininess(shininess),
			m_texture(tex),
			m_shader_program(shader_program)
		{}

		~material() = default;

		math::vec4 m_color;

		int32_t m_diffuse;

		int32_t m_specular;

		int32_t m_ambient;

		float32_t m_shininess;

		texture& m_texture;

		GLuint m_shader_program;
	};


	struct mesh_component
	{
		mesh_component() = default;

		mesh_component(const std::string& name) :
			m_mesh_name(name)
		{}

		~mesh_component() = default;

		bool operator!=(const mesh_component& other) const
		{
			return m_mesh_name != other.m_mesh_name;
		}

		std::string m_mesh_name = "";
	};

	struct material_component
	{
		material_component() = default;

		material_component(const std::string& name) :
			m_material_name(name)
		{}

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
			m_renderable(has_mesh&& has_transform&& has_material)
		{}

		bool operator!=(const render_component& other) const
		{
			return m_renderable != other.m_renderable;
		}

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
			std::optional <int32_t> depth = std::nullopt,
			const std::vector <std::pair<GLenum, GLenum>>& params = {
				{GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
				{GL_TEXTURE_MAG_FILTER, GL_LINEAR},
				{GL_TEXTURE_WRAP_S, GL_REPEAT},
				{GL_TEXTURE_WRAP_T, GL_REPEAT}
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

			std::vector<std::pair<GLenum, GLenum>> default_params = {
				{GL_TEXTURE_MIN_FILTER, GL_LINEAR},
				{GL_TEXTURE_MAG_FILTER, GL_LINEAR},
				{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},
				{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE}
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
			const std::pair <std::optional<std::string>, std::optional<std::string>>&shader_paths = {std::nullopt, std::nullopt},
			const math::vec4& color = math::vec4(1),
			const std::string& texture_name = "",
			int32_t diffuse = 0,
			int32_t specular = 0,
			int32_t ambient = 1,
			float32_t shininess = 0)
		{
			texture* tex = nullptr;

			GLuint shader_program = std::get<1>(m_shader_manager.load_shader(shader_paths));

			if (!texture_name.empty() && m_texture_manager.get_textures().contains(texture_name))
			{
				tex = &(m_texture_manager.get_texture(texture_name));
			}
			else
			{
				tex = &(m_texture_manager.create_default_texture());
			}

			m_materials[name] = std::make_unique<material>(
				shader_program,
				color,
				*tex,
				diffuse,
				specular,
				ambient,
				shininess
			);

			return *m_materials[name];
		}

		void load_material(const std::string& name, std::unique_ptr<material> mat)
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
			std::optional <int32_t> depth = std::nullopt,
			const std::vector <std::pair<GLenum, GLenum>>& params = {
				{GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR},
				{GL_TEXTURE_MAG_FILTER, GL_LINEAR},
				{GL_TEXTURE_WRAP_S, GL_REPEAT},
				{GL_TEXTURE_WRAP_T, GL_REPEAT}
			})
		{
		return m_texture_manager.create_texture(name, path, type, depth, params);
		}

	private:
		material_manager() = default;
		~material_manager() = default;
	private:
		shader_manager& m_shader_manager = shader_manager::get();
		texture_manager& m_texture_manager = texture_manager::get();
		std::unordered_map<std::string, std::unique_ptr<material>> m_materials;
	};


	class mesh_manager
	{
	public:
		static mesh_manager& get()
		{
			static mesh_manager instance;
			return instance;
		}

		mesh& create_mesh(const std::string& name, const std::vector <vertex>& vertices)
		{
			m_meshes[name] = std::make_unique<mesh>(vertices);
			return *m_meshes[name];
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

	void set_uniform(const material& material, GLuint shader_program);

	void set_uniform(const std::string& uniform_name, const material& material, GLuint shader_program);

	// ==============================================================================

	// Types
	using transform_component = transform;
}

#endif