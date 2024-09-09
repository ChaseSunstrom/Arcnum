#ifndef SPARK_MODEL_HPP
#define SPARK_MODEL_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>
#include <core/util/memory/const_ptr.hpp>
#include <core/util/memory/ref_ptr.hpp>
#include "material.hpp"
#include "mesh.hpp"

namespace Spark {
	class StaticModel {
	public:
		friend class Manager<StaticModel>;
		~StaticModel() = default;

		ConstPtr<StaticMesh> GetMesh() const { return m_mesh; }
		ConstPtr<Material>   GetMaterial() const { return m_material; }

		void SetMaterial(Material* material) { m_material = material; }

	private:
		StaticModel(ConstPtr<StaticMesh> mesh, RefPtr<Material> material = nullptr)
			: m_mesh(mesh)
			, m_material(material) {}

	private:
		ConstPtr<StaticMesh> m_mesh;
		RefPtr<Material>     m_material;
	};

	class DynamicModel {
	public:
		friend class Manager<DynamicModel>;
		~DynamicModel() = default;

		DynamicModel(const DynamicModel& other);
		DynamicModel& operator=(const DynamicModel& other);

		ConstPtr<DynamicMesh> GetMesh() const { return m_mesh; }
		ConstPtr<Material>    GetMaterial() const { return m_material; }

		void SetMaterial(RefPtr<Material> material) { m_material = material; }

	private:
		DynamicModel(std::unique_ptr<DynamicMesh> mesh, RefPtr<Material> material = nullptr)
			: m_mesh(std::move(mesh))
			, m_material(material) {}

	private:
		std::unique_ptr<DynamicMesh> m_mesh;
		RefPtr<Material>             m_material;
	};
} // namespace Spark

#endif // SPARK_MODEL_HPP