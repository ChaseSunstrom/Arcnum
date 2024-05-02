#ifndef SPARK_COMPONENT_TYPES_HPP
#define SPARK_COMPONENT_TYPES_HPP

#include "../../spark.hpp"
#include "shader.hpp"
#include "mesh.hpp"
#include "../../window/window_type.hpp"
#include "../../window/vulkan/vulkan_mesh.hpp"

namespace spark
{
	enum class TextureType
	{
		TWO_D = 2, THREE_D = 3
	};

	struct Transform
	{
		Transform(
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

		Transform& operator*=(const Transform& rhs)
		{
			this->m_transform = this->m_transform * rhs.m_transform;
			return *this;
		}

		Transform operator*(const Transform& rhs) const
		{
			Transform result = *this; // Copy current transform
			result *= rhs;            // Apply multiplication
			return result;
		}

		math::mat4 m_transform;
	};

	class Texture
	{
	public:
		Texture() = default;

	private:

	};

	struct Material
	{
		Material(
			const math::vec4& color, Texture& tex, i32 diffuse, i32 specular, i32 ambient, f32 shininess) :
			m_color(color), m_diffuse(diffuse), m_specular(specular), m_ambient(ambient), m_shininess(shininess), m_texture(
				tex) { }

		~Material() = default;

		math::vec4 m_color;

		i32 m_diffuse;

		i32 m_specular;

		i32 m_ambient;

		f32 m_shininess;

		Texture& m_texture;
	};

	struct MeshComponent
	{
		MeshComponent() = default;

		MeshComponent(const std::string& name) :
			m_mesh_name(name) { }

		~MeshComponent() = default;

		std::string m_mesh_name = "";
	};

	struct MaterialComponent
	{
		MaterialComponent() = default;

		MaterialComponent(const std::string& name) :
			m_material_name(name) { }

		~MaterialComponent() = default;

		bool operator!=(const MaterialComponent& other) const
		{
			return m_material_name != other.m_material_name;
		}

		std::string m_material_name = "";
	};

	class TextureManager
	{
	public:
		static TextureManager& get()
		{
			static TextureManager instance;
			return instance;
		}

		void destroy_texture(const std::string& name);

		Texture& get_texture(const std::string& name);

		Texture& create_texture(
			const std::string& name,
			const std::string& path,
			TextureType type = TextureType::TWO_D,
			bool flip = true);

		Texture& create_default_texture();

		std::unordered_map<std::string, std::unique_ptr<Texture>>& get_textures();

	private:
		TextureManager() = default;

		~TextureManager() = default;

	private:
		std::unordered_map <std::string, std::unique_ptr<Texture>> m_textures;
	};

	class MaterialManager
	{
	public:
		static MaterialManager& get()
		{
			static MaterialManager instance;
			return instance;
		}

		Material& create_material(
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

			Texture* tex = nullptr;

			if (!texture_name.empty() && m_texture_manager.get_textures().contains(texture_name))
			{
				tex = &(m_texture_manager.get_texture(texture_name));
			}
			else
			{
				tex = &(m_texture_manager.create_default_texture());
			}

			m_materials[name] = std::make_unique<Material>(
				color, *tex, diffuse, specular, ambient, shininess);

			return *m_materials[name];
		}

		void load_material(const std::string& name, std::unique_ptr <Material> mat)
		{
			m_materials[name] = std::move(mat);
		}

		Material& get_material(const std::string& name)
		{
			return *m_materials.at(name);
		}

		void destroy_material(const std::string& name)
		{
			m_materials.erase(name);
		}

	private:
		MaterialManager()
		{
			create_material("__default__");
		}

		~MaterialManager() = default;

	private:
		ShaderManager& m_shader_manager = ShaderManager::get();

		TextureManager& m_texture_manager = TextureManager::get();

		std::unordered_map <std::string, std::unique_ptr<Material>> m_materials;
	};

	class MeshManager
	{
	public:
		static MeshManager& get()
		{
			static MeshManager instance;
			return instance;
		}

		template <typename... Args>
		Mesh& create_mesh(
			const std::string& name,
			const std::vector <Vertex>& vertices,
			const std::vector <u32> indices = { },
			Args&& ... args)
		{
			if (m_meshes.contains(name))
			{
				return *m_meshes[name];
			}

			if (get_current_window_type() == WindowType::VULKAN)
			{
				m_meshes[name] = std::make_unique<VulkanMesh>(vertices, indices, std::forward<Args>(args)...);
				return *m_meshes[name];
			}
		}

		void load_mesh(const std::string& name, std::unique_ptr <Mesh> mesh)
		{
			m_meshes[name] = std::move(mesh);
		}

		Mesh& get_mesh(const std::string& name)
		{
			return *m_meshes[name];
		}

		void destroy_mesh(const std::string& name)
		{
			m_meshes.erase(name);
		}

	private:
		MeshManager() = default;

		~MeshManager() = default;

	private:
		std::unordered_map <std::string, std::unique_ptr<Mesh>> m_meshes;
	};

	// ==============================================================================
	// GLSL FUNCTIONS: 

	void set_uniform(const Material& material, u32 shader_program);

	void set_uniform(const std::string& uniform_name, const Material& material, u32 shader_program);

	// ==============================================================================

	// Types
	using TransformComponent = Transform;
}

#endif