#ifndef SPARK_SCENE_HPP
#define SPARK_SCENE_HPP

#include "octree.hpp"
#include <core/pch.hpp>
#include <core/system/manager.hpp>

namespace Spark {
class Scene {
  public:
	friend class Manager<Scene>;
	~Scene() = default;

	void OnEvent(const std::shared_ptr<ComponentEvent<TransformComponent>> event);
	void Insert(const PointData& point_data);
	void Update(const PointData& point_data);
	void Remove(i64 entity_id);


	Octree& GetOctree() { return *m_octree; }
  private:
	Scene(const glm::vec3& center = glm::vec3(0), f32 width = 100)
		: m_octree(std::make_unique<Octree>(center, width)) {}
  private:
	std::unique_ptr<Octree> m_octree;
};

// Deleted Copy functions
template <>
class Manager<Scene> : public IManager {
  public:
	Manager()
		: m_registry(std::make_unique<Registry<Scene>>()) {}
	~Manager() = default;

	template <typename... Args>
	Scene& Create(const std::string& name, Args&&... args) {
		Scene* object = new Scene(std::forward<Args>(args)...);
		Register(name, std::unique_ptr<Scene>(object));

		if (GetSize() == 1)
			m_current_scene = &Get(name);

		return *m_current_scene;
	}

	void OnEvent(const std::shared_ptr<ComponentEvent<TransformComponent>> event) {
		m_current_scene->OnEvent(event);
	}

	Scene& Register(const std::string& name, std::unique_ptr<Scene> object) { return m_registry->Register(name, std::move(object)); }

	Scene& Get(const std::string& name) const { return m_registry->Get(name); }

	Scene& Get(const Handle handle) const { return m_registry->Get(handle); }

	Scene& GetCurrentScene() const { return *m_current_scene; }
	Scene& SetCurrentScene(const std::string& name) {
		m_current_scene = &m_registry->Get(name);
		return *m_current_scene;
	} 

	void Remove(const std::string& name) { m_registry->Remove(name); }

	void Remove(const Handle handle) { m_registry->Remove(handle); }

	size_t GetSize() const { return m_registry->GetSize(); }

	std::vector<std::string> GetKeys() const { return m_registry->GetKeys(); }

	void SetRegistry(std::unique_ptr<Registry<Scene>> registry) { m_registry = std::move(registry); }

	Registry<Scene>& GetRegistry() const { return *m_registry; }

  private:
	Scene* m_current_scene;
	std::unique_ptr<Registry<Scene>> m_registry;
};
} // namespace Spark

#endif