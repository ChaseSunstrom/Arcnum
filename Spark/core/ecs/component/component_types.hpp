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

		GLenum get_gl_texture_type() const;

	private:
		uint32_t m_texture;

		void* m_image_data = nullptr;

		int32_t m_width;

		int32_t m_height;

		int32_t m_nr_channels;

		std::optional <int32_t> m_depth;

		texture_type m_type;

		void load_texture(const std::filesystem::path& path, const std::vector<std::pair<GLenum, GLenum>>& params);

		void generate_texture(GLenum target, GLenum internal_format, GLenum format, GLenum type);

		void set_texture_parameters(GLenum target, const std::vector<std::pair<GLenum, GLenum>>& params);
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

	struct instanced_mesh
	{
		instanced_mesh(mesh&& m) :
			m_mesh(std::move(m)), m_instance_vbo(0)
		{
			generate_vertex_buffer(m_instance_vbo);
		}

		~instanced_mesh()
		{
			delete_vertex_buffer(m_instance_vbo);
		}

		instanced_mesh(const instanced_mesh&) = delete;

		instanced_mesh& operator=(const instanced_mesh&) = delete;

		void update_instance_data(const std::vector <transform>& transforms)
		{
			m_instance_transforms = transforms;
			bind_index_buffer(m_instance_vbo);
			buffer_vertex_data<transform>(m_instance_vbo, m_instance_transforms);
		}

		mesh m_mesh;

		std::vector<transform> m_instance_transforms;

	private:
		GLuint m_instance_vbo;
	};

	// Yes this is big, I know
	struct material
	{
		material() = default;

		material(
			GLuint shader_program,
			const math::vec4& color = math::vec4(1),
			texture texture = texture(),
			int32_t diffuse = 0,
			int32_t specular = 0,
			int32_t ambient = 1,
			float32_t shininess = 0) :
			m_color(color), m_diffuse(diffuse), m_specular(specular), m_ambient(ambient), m_shininess(shininess), m_texture(texture),
			m_shader_program(shader_program)
		{}

		~material() = default;

		math::vec4 m_color = math::vec4(1);

		int32_t m_diffuse;

		int32_t m_specular;

		int32_t m_ambient;

		float32_t m_shininess;

		texture m_texture;

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

	class material_manager
	{
	public:
		material_manager() = default;

		~material_manager() = default;

		template <typename... Args>
		material& create_material(
			const std::string& name,
			const std::pair <std::optional<std::string>,
			std::optional<std::string>>&shader_paths,
			Args&& ... args)
		{
			GLuint shader_program = std::get<1>(m_shader_manager->load_shader(shader_paths));

			m_materials[name] = std::make_unique<material>(shader_program, std::forward<Args>(args)...);
			return *m_materials[name];
		}

		void load_material(const std::string& name, std::unique_ptr<material> material);

		material& get_material(const std::string& name);

		void destroy_material(const std::string& name);
	private:
		std::unique_ptr <shader_manager> m_shader_manager = std::make_unique<shader_manager>();

		std::unordered_map <std::string, std::unique_ptr<material>> m_materials = std::unordered_map<std::string, std::unique_ptr<material>>();
	};

	class texture_manager
	{
	public:
		texture_manager() = default;

		~texture_manager() = default;

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
			m_textures[name] = texture(path, type, depth, params);
			return m_textures[name];
		}

		void load_texture(const std::string& name, const texture& texture);

		texture& get_texture(const std::string& name);

		void destroy_texture(const std::string& name);

	private:
		std::unordered_map <std::string, texture> m_textures;
	};

	class mesh_manager
	{
	public:
		mesh_manager() = default;

		~mesh_manager() = default;

		instanced_mesh& create_mesh(const std::string& name, const std::vector <vertex>& vertices)
		{
			m_meshes[name] = std::make_unique<instanced_mesh>(mesh(vertices));
			return *m_meshes[name];
		}

		void load_mesh(const std::string& name, std::unique_ptr <instanced_mesh> instanced_mesh)
		{
			m_meshes[name] = std::move(instanced_mesh);
		}

		instanced_mesh& get_mesh(const std::string& name)
		{
			return *m_meshes[name];
		}

		void destroy_mesh(const std::string& name)
		{
			m_meshes.erase(name);
		}

	private:
		std::unordered_map <std::string, std::unique_ptr<instanced_mesh>> m_meshes;
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