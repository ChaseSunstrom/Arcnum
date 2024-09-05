#ifndef SPARK_MODEL_HPP
#define SPARK_MODEL_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>
#include "material.hpp"
#include "mesh.hpp"

namespace Spark {

	class StaticModel {
	  public:
		friend class Manager<StaticModel>;
		~StaticModel() = default;

		const StaticMesh& GetMesh() const { return m_mesh; }
		const Material*   GetMaterial() const { return m_material; }
		const glm::mat4&  GetModelMatrix() const { return m_model_matrix; }

		void SetMaterial(Material* material) { m_material = material; }
		void SetModelMatrix(const glm::mat4& matrix) { m_model_matrix = matrix; }

	  private:
		StaticModel(const StaticMesh& mesh, Material* material = nullptr)
			: m_mesh(mesh)
			, m_material(material)
			, m_model_matrix(1.0f) {}

	  private:
		const StaticMesh& m_mesh;
		Material*         m_material;
		glm::mat4         m_model_matrix;
	};

	class DynamicModel {
	  public:
		friend class Manager<DynamicModel>;
		~DynamicModel() = default;

		DynamicModel(const DynamicModel& other);
		DynamicModel& operator=(const DynamicModel& other);

		const DynamicMesh& GetMesh() const { return *m_mesh; }
		const Material*    GetMaterial() const { return m_material; }
		const glm::mat4&   GetModelMatrix() const { return m_model_matrix; }

		void SetMaterial(Material* material) { m_material = material; }
		void SetModelMatrix(const glm::mat4& matrix) { m_model_matrix = matrix; }

	  private:
		DynamicModel(std::unique_ptr<DynamicMesh> mesh, Material* material = nullptr)
			: m_mesh(std::move(mesh))
			, m_material(material)
			, m_model_matrix(1.0f) {}

	  private:
		std::unique_ptr<DynamicMesh> m_mesh;
		Material*                    m_material;
		glm::mat4                    m_model_matrix;
	};

} // namespace Spark

#endif // SPARK_MODEL_HPP