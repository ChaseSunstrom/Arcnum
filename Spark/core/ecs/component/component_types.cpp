#include "component_types.hpp"

#include "../../util/wrap.hpp"

// Because stb_image is fucking weird
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace spark
{
	mesh::mesh()
	{
		create_mesh();
	}

	mesh::mesh(
		const std::vector <vertex>& vertices
	)
	{
		m_vertices = vertices;

		create_mesh();
	}

	void mesh::create_mesh()
	{
		// Generate and bind VAO
		generate_vertex_array(m_vao);
		bind_vertex_array(m_vao);

		// Generate and bind VBO
		generate_vertex_buffer(m_vbo);
		bind_vertex_buffer(m_vbo);
		buffer_vertex_data<vertex>(m_vbo, m_vertices);

		set_vertex_attribute_ptr(0, 3, GL_FLOAT, sizeof vertex, (void*)0);
		enable_vertex_attribute_ptr(0);

		set_vertex_attribute_ptr(
			1, 3, GL_FLOAT, sizeof vertex, (void*)offsetof(vertex, m_normal)
		);
		enable_vertex_attribute_ptr(1);

		set_vertex_attribute_ptr(
			2, 2, GL_FLOAT, sizeof vertex, (void*)offsetof(vertex, m_texcoords)
		);
		enable_vertex_attribute_ptr(2);

		bind_vertex_array(0);
	}


	mesh::~mesh()
	{
		delete_vertex_buffer(m_vbo);
		delete_vertex_array(m_vao);
	}

	void mesh::bind()
	{
		bind_vertex_array(m_vao);
	}

	void mesh::unbind()
	{
		bind_vertex_array(0);
	}

	texture::texture(
		const std::filesystem::path& path,
		texture_type type,
		std::optional <int32_t> depth,
		const std::vector <std::pair<GLenum, GLenum>>& params
	) : m_type(type), m_depth(depth)
	{
		spark::generate_texture(m_texture);
		load_texture(path, params);
	}

	texture::~texture()
	{
		if (m_image_data)
		{
			stbi_image_free(m_image_data);
		}

		glDeleteTextures(1, &m_texture);
	}

	GLenum texture::get_gl_texture_type() const
	{
		return (m_type == texture_type::TWO_D) ? GL_TEXTURE_2D : GL_TEXTURE_3D;
	}

	void texture::load_texture(const std::filesystem::path& path, const std::vector <std::pair<GLenum, GLenum>>& params)
	{
		m_image_data = stbi_load(path.string().c_str(), &m_width, &m_height, &m_nr_channels, 0);
		if (!m_image_data)
		{
			throw std::runtime_error("Failed to load texture");
		}

		GLenum format = GL_RGB; // Default format, adjust based on m_nr_channels if needed
		GLenum target = (m_type == texture_type::TWO_D) ? GL_TEXTURE_2D : GL_TEXTURE_3D;

		bind(); // Bind before setting texture parameters and uploading data
		generate_texture(target, format, format, GL_UNSIGNED_BYTE);
		set_texture_parameters(target, params);
	}

	void texture::generate_texture(GLenum target, GLenum internal_format, GLenum format, GLenum type)
	{
		if (m_type == texture_type::TWO_D)
		{
			glTexImage2D(target, 0, internal_format, m_width, m_height, 0, format, type, m_image_data);
			generate_mipmap(target);
		}
		else if (m_type == texture_type::THREE_D && m_depth.has_value())
		{
			// Use the depth value if provided for 3D textures
			glTexImage3D(target, 0, internal_format, m_width, m_height, m_depth.value(), 0, format, type, m_image_data);
			generate_mipmap(target);
		}
	}

	void texture::set_texture_parameters(GLenum target, const std::vector <std::pair<GLenum, GLenum>>& params)
	{
		for (const auto& [pname, param] : params)
		{
			glTexParameteri(target, pname, param);
		}
	}

	void texture::bind(GLenum texture_unit)
	{
		active_texture(texture_unit);
		bind_texture((m_type == texture_type::TWO_D) ? GL_TEXTURE_2D : GL_TEXTURE_3D, m_texture);
	}

	void material_manager::load_material(
		const std::string& name, std::unique_ptr<material> material
	)
	{
		m_materials[name] = std::move(material);
	}

	material& material_manager::get_material(const std::string& name)
	{
		return *m_materials[name];
	}

	void material_manager::destroy_material(const std::string& name)
	{
		m_materials.erase(name);
	}

	void texture_manager::load_texture(
		const std::string& name, const texture& texture
	)
	{
		m_textures[name] = texture;
	}

	void texture_manager::destroy_texture(const std::string& name)
	{
		m_textures.erase(name);
	}

	texture& texture_manager::get_texture(const std::string& name)
	{
		return m_textures[name];
	}

	void set_uniform(const material& material, GLuint shader_program)
	{
		set_uniform("material", material, shader_program);
	}

	void set_uniform(
		const std::string& uniform_name,
		const material& material,
		GLuint shader_program
	)
	{
		int32_t texture_unit = 0;

		GLenum texture_type = material.m_texture.get_gl_texture_type();

		glUseProgram(shader_program);

		// Automatically bind textures to texture units and set the corresponding
		// uniforms=
		//active_texture(GL_TEXTURE0 + texture_unit);       // Activate texture unit
		//bind_texture(texture_type, material.m_diffuse);  // Bind the diffuse texture
		set_uniform(uniform_name + ".diffuse", texture_unit, shader_program);  // Set the sampler to use this texture unit
		texture_unit++;               // Move to the next texture unit

		/*active_texture(GL_TEXTURE0 + texture_unit);
		bind_texture(texture_type, material.m_specular);
		*/
		set_uniform(uniform_name + ".specular", texture_unit, shader_program);
		texture_unit++;

		//active_texture(GL_TEXTURE0 + texture_unit);
		//bind_texture(texture_type, material.m_ambient);
		set_uniform(uniform_name + ".ambient", texture_unit, shader_program);
		texture_unit++;

		// Set other material properties	
		set_uniform(uniform_name + ".shininess", material.m_shininess, shader_program);
		set_uniform(uniform_name + ".color", material.m_color, shader_program);
	}
}  // namespace spark