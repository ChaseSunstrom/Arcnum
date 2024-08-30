#ifndef SPARK_MESH_HPP
#define SPARK_MESH_HPP

#include "vertex.hpp"
#include <core/pch.hpp>
#include <core/system/manager.hpp>

namespace Spark {
class StaticMesh {
  public:
	friend class Manager<StaticMesh>;
	virtual ~StaticMesh()                    = default;
	StaticMesh(const StaticMesh&)            = delete;
	StaticMesh& operator=(const StaticMesh&) = delete;
	virtual void CreateMesh()                = 0;
	const std::vector<Vertex>& GetVertices() const { return m_vertices; }
	const std::vector<u32>& GetIndices() const { return m_indices; }

  protected:
	StaticMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices)
		: m_vertices(vertices)
		, m_indices(indices) {}

  protected:
	std::vector<Vertex> m_vertices;
	std::vector<u32> m_indices;
};

class DynamicModel;

class DynamicMesh {
  public:
	// These two things need to be able to create these (mainly for when copying)
	friend class Manager<DynamicMesh>;
	friend class DynamicModel;
	virtual ~DynamicMesh() = default;
	virtual void Update(const std::vector<Vertex>& vertices, const std::vector<u32>& indices);
	virtual void CreateMesh();
	virtual void UpdateMesh(const std::vector<Vertex>& vertices);
	virtual void UpdateIndices(const std::vector<u32>& indices);
	const std::vector<Vertex>& GetVertices() const { return m_vertices; }
	const std::vector<u32>& GetIndices() const { return m_indices; }

  protected:
	DynamicMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices)
		: m_vertices(vertices)
		, m_indices(indices) {}

  protected:
	std::vector<Vertex> m_vertices;
	std::vector<u32> m_indices;
};

// Removed GetCopy function
template <> class Manager<StaticMesh> {
  public:
	Manager()
		: m_registry(std::make_unique<Registry<StaticMesh>>()) {}
	~Manager() = default;

	template <typename... Args> Handle Create(const std::string& name, Args&&... args) {
		StaticMesh* object = new StaticMesh(std::forward<Args>(args)...);
		return Register(name, std::unique_ptr<StaticMesh>(object));
	}

	Handle Register(const std::string& name, std::unique_ptr<StaticMesh> object) { return m_registry->Register(name, std::move(object)); }

	StaticMesh& Get(const std::string& name) const { return m_registry->Get(name); }

	StaticMesh& Get(const Handle handle) const { return m_registry->Get(handle); }

	void Remove(const std::string& name) { m_registry->Remove(name); }

	void Remove(const Handle handle) { m_registry->Remove(handle); }

	std::vector<std::string> GetKeys() const { return m_registry->GetKeys(); }

	void SetRegistry(std::unique_ptr<Registry<StaticMesh>> registry) { m_registry = std::move(registry); }

	Registry<StaticMesh>& GetRegistry() const { return *m_registry; }

  private:
	std::unique_ptr<Registry<StaticMesh>> m_registry;
};
} // namespace Spark

#endif