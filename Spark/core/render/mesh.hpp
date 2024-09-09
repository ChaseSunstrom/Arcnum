#ifndef SPARK_MESH_HPP
#define SPARK_MESH_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>
#include "vertex.hpp"

namespace Spark {
	class StaticMesh {
	public:
		friend class Manager<StaticMesh>;
		virtual ~StaticMesh()                    = default;
		StaticMesh(const StaticMesh&)            = delete;
		StaticMesh& operator=(const StaticMesh&) = delete;

		virtual void               CreateMesh() = 0;
		const std::vector<Vertex>& GetVertices() const { return m_vertices; }
		const std::vector<u32>&    GetIndices() const { return m_indices; }
		size_t                     GetVerticesSize() const { return m_vertices.size(); }
		size_t                     GetIndicesSize() const { return m_indices.size(); }

		// API-specific methods (to be implemented in derived classes)
		virtual u32 GetVAO() const = 0;
		virtual u32 GetVBO() const = 0;
		virtual u32 GetEBO() const = 0;

	protected:
		StaticMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices)
			: m_vertices(vertices)
			, m_indices(indices) {}

	protected:
		std::vector<Vertex> m_vertices;
		std::vector<u32>    m_indices;
	};

	class DynamicMesh {
	public:
		friend class Manager<DynamicMesh>;
		friend class DynamicModel;
		virtual ~DynamicMesh() = default;

		virtual void Update(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) = 0;
		virtual void CreateMesh() = 0;
		virtual void UpdateMesh(const std::vector<Vertex>& vertices) = 0;
		virtual void UpdateIndices(const std::vector<u32>& indices) = 0;

		const std::vector<Vertex>& GetVertices() const { return m_vertices; }
		const std::vector<u32>&    GetIndices() const { return m_indices; }
		size_t                     GetVerticesSize() const { return m_vertices.size(); }
		size_t                     GetIndicesSize() const { return m_indices.size(); }

		// API-specific methods (to be implemented in derived classes)
		virtual u32 GetVAO() const = 0;
		virtual u32 GetVBO() const = 0;
		virtual u32 GetEBO() const = 0;

	protected:
		DynamicMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices)
			: m_vertices(vertices)
			, m_indices(indices) {}

	protected:
		std::vector<Vertex> m_vertices;
		std::vector<u32>    m_indices;
	};

	template<> class Manager<StaticMesh> : public IManager {
	public:
		Manager() : m_registry(std::make_unique<Registry<StaticMesh>>()) {}
		~Manager() = default;

		template<typename DerivedMesh, typename... Args> RefPtr<DerivedMesh> Create(const std::string& name, Args&&... args) {
			auto* mesh = new DerivedMesh(std::forward<Args>(args)...);
			return RefCast<DerivedMesh>(Register(name, std::unique_ptr<DerivedMesh>(mesh)));
		}

		RefPtr<StaticMesh>       Register(const std::string& name, std::unique_ptr<StaticMesh> object) { return m_registry->Register(name, std::move(object)); }
		RefPtr<StaticMesh>       Get(const std::string& name) const { return m_registry->Get(name); }
		RefPtr<StaticMesh>       Get(const Handle handle) const { return m_registry->Get(handle); }
		void                     Remove(const std::string& name) { m_registry->Remove(name); }
		void                     Remove(const Handle handle) { m_registry->Remove(handle); }
		size_t                   GetSize() const { return m_registry->GetSize(); }
		std::vector<std::string> GetKeys() const { return m_registry->GetKeys(); }
		void                     SetRegistry(std::unique_ptr<Registry<StaticMesh>> registry) { m_registry = std::move(registry); }
		Registry<StaticMesh>&    GetRegistry() const { return *m_registry; }

	private:
		std::unique_ptr<Registry<StaticMesh>> m_registry;
	};
} // namespace Spark

#endif // SPARK_MESH_HPP