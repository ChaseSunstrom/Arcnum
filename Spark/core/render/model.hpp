#ifndef SPARK_MODEL_HPP
#define SPARK_MODEL_HPP

#include "material.hpp"
#include "mesh.hpp"
#include <core/pch.hpp>
#include <core/system/manager.hpp>

namespace Spark {
// Static models will hold a reference to a static mesh, dynamic ones will
// store their own local copy of the mesh which will take longer to create
// because it needs to copy all the mesh data

// TODO: Make Materials implementation independent?

class StaticModel {
  public:
	friend class Manager<StaticModel>;
	~StaticModel() = default;

  private:
	StaticModel(const StaticMesh& mesh, std::vector<std::reference_wrapper<Material>> materials)
		: m_mesh(mesh)
		, m_materials(materials) {}

  private:
	const StaticMesh& m_mesh;
	std::vector<std::reference_wrapper<Material>> m_materials;
};

class DynamicModel {
  public:
	friend class Manager<DynamicModel>;
	~DynamicModel() = default;
	DynamicModel(const DynamicModel& other) {
		DynamicMesh* mesh = new DynamicMesh(other.m_mesh->GetVertices(), other.m_mesh->GetIndices());
		m_mesh            = std::unique_ptr<DynamicMesh>(mesh);
		m_materials       = other.m_materials;
	}
	DynamicModel& operator=(const DynamicModel& other) {
		DynamicMesh* mesh = new DynamicMesh(other.m_mesh->GetVertices(), other.m_mesh->GetIndices());
		m_mesh            = std::unique_ptr<DynamicMesh>(mesh);
		m_materials       = other.m_materials;
		return *this;
	}

  private:
	DynamicModel(std::unique_ptr<DynamicMesh> mesh, const std::vector<std::reference_wrapper<Material>>& materials)
		: m_mesh(std::move(mesh))
		, m_materials(materials) {}

  private:
	std::unique_ptr<DynamicMesh> m_mesh;
	std::vector<std::reference_wrapper<Material>> m_materials;
};
} // namespace Spark

#endif