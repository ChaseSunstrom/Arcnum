#ifndef SPARK_RESOURCE_HPP
#define SPARK_RESOURCE_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>
#include <core/render/mesh.hpp>
#include <core/render/material.hpp>
#include <core/render/model.hpp>
#include <core/render/shader.hpp>

namespace Spark
{
	// Dummy class for all resources, which includes, meshes, materials, models, shaders, etc.
	class Resource
	{
	public:
		virtual ~Resource() = default;
	};

	// Overload for Manager<Resource>
	template <>
	class Manager<Resource>
	{
	public:
		// Shader management
		template<IsShader T, typename... Args>
		T& CreateShader(const std::string& name, Args&&... args) {
			return m_shader_manager.Create<T>(name, std::forward<Args>(args)...);
		}

		Shader& GetShader(const std::string& name) const {
			return m_shader_manager.Get(name);
		}

		// Material management
		Material& CreateMaterial(const std::string& name, const MaterialData& material_data, const std::string& shader_name) {
			return m_material_manager.Create(name, material_data, shader_name);
		}

		Material& GetMaterial(const std::string& name) const {
			return m_material_manager.Get(name);
		}

		// Static Mesh management
		StaticMesh& CreateStaticMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
			auto& mesh = m_static_mesh_manager.Create(name, vertices, indices);
			mesh.CreateMesh();
			return mesh;
		}

		StaticMesh& GetStaticMesh(const std::string& name) const {
			return m_static_mesh_manager.Get(name);
		}

		// Dynamic Mesh management
		DynamicMesh& CreateDynamicMesh(const std::string& name, const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
			auto& mesh = m_dynamic_mesh_manager.Create(name, vertices, indices);
			mesh.CreateMesh();
			return mesh;
		}

		DynamicMesh& GetDynamicMesh(const std::string& name) const {
			return m_dynamic_mesh_manager.Get(name);
		}

		// Static Model management
		StaticModel& CreateStaticModel(const std::string& name, const std::string& mesh_name, const std::vector<std::string>& material_names) {
			const auto& mesh = GetStaticMesh(mesh_name);
			std::vector<std::reference_wrapper<Material>> materials;
			for (const auto& mat_name : material_names) {
				materials.emplace_back(GetMaterial(mat_name));
			}
			return m_static_model_manager.Create(name, mesh, materials);
		}

		StaticModel& GetStaticModel(const std::string& name) const {
			return m_static_model_manager.Get(name);
		}

		// Dynamic Model management
		DynamicModel& CreateDynamicModel(const std::string& name, const std::string& mesh_name, const std::vector<std::string>& material_names) {
			const auto& mesh = GetDynamicMesh(mesh_name);
			std::vector<std::reference_wrapper<Material>> materials;
			for (const auto& mat_name : material_names) {
				materials.emplace_back(GetMaterial(mat_name));
			}
			return m_dynamic_model_manager.Create(name, std::make_unique<DynamicMesh>(mesh.GetVertices(), mesh.GetIndices()), materials);
		}

		DynamicModel& GetDynamicModel(const std::string& name) const {
			return m_dynamic_model_manager.Get(name);
		}
	private:
		Manager<StaticMesh> m_static_mesh_manager;
		Manager<DynamicMesh> m_dynamic_mesh_manager;
		Manager<Material> m_material_manager;
		Manager<StaticModel> m_static_model_manager;
		Manager<DynamicModel> m_dynamic_model_manager;
		Manager<Shader> m_shader_manager;
	};
}

#endif // SPARK_RESOURCE_HPP