#ifndef SPARK_RESOURCE_HPP
#define SPARK_RESOURCE_HPP

#include <core/pch.hpp>
#include <core/render/material.hpp>
#include <core/render/mesh.hpp>
#include <core/render/model.hpp>
#include <core/render/shader.hpp>
#include <core/scene/scene.hpp>
#include <core/system/manager.hpp>

namespace Spark {
// Dummy class for all resources, which includes, meshes, materials, models,
// shaders, etc.
class Resource {
  public:
	virtual ~Resource() = default;
};

// Overload for Manager<Resource>
template <>
class Manager<Resource> {
  public:
	Manager() {
		m_managers[typeid(StaticMesh)]   = std::make_unique<Manager<StaticMesh>>();
		m_managers[typeid(DynamicMesh)]  = std::make_unique<Manager<DynamicMesh>>();
		m_managers[typeid(Material)]     = std::make_unique<Manager<Material>>();
		m_managers[typeid(StaticModel)]  = std::make_unique<Manager<StaticModel>>();
		m_managers[typeid(DynamicModel)] = std::make_unique<Manager<DynamicModel>>();
		m_managers[typeid(Shader)]       = std::make_unique<Manager<Shader>>();
		m_managers[typeid(Scene)]        = std::make_unique<Manager<Scene>>();
	}

	~Manager() = default;

	template <typename T, typename... Args>
	T& Create(const std::string& name, Args&&... args) {
		auto& manager = GetManager<T>();
		return manager.Create(name, std::forward<Args>(args)...);
	}

	template <typename T>
	T& Register(const std::string& name, std::unique_ptr<T> object) {
		auto& manager = GetManager<T>();
		return manager.Register(name, std::move(object));
	}

	template <typename T>
	T& Get(const std::string& name) {
		auto& manager = GetManager<T>();
		return manager.Get(name);
	}

	template <typename T>
	T& Get(const Handle handle) {
		auto& manager = GetManager<T>();
		return manager.Get(handle);
	}

	template <typename T>
	T GetCopy(const std::string& name) {
		auto& manager = GetManager<T>();
		return manager.GetCopy(name);
	}

	template <typename T>
	T GetCopy(const Handle handle) {
		auto& manager = GetManager<T>();
		return manager.GetCopy(handle);
	}

	template <typename T>
	void Remove(const std::string& name) {
		auto& manager = GetManager<T>();
		manager.Remove(name);
	}

	template <typename T>
	void Remove(const Handle handle) {
		auto& manager = GetManager<T>();
		manager.Remove(handle);
	}

	template <typename T>
	std::vector<std::string> GetKeys() {
		auto& manager = GetManager<T>();
		return manager.GetKeys();
	}

	template <typename T>
	void SetRegistry(std::unique_ptr<Registry<T>> registry) {
		auto& manager = GetManager<T>();
		manager.SetRegistry(std::move(registry));
	}

	template <typename T>
	Manager<T>& GetManager() {
		auto it = m_managers.find(typeid(T));
		if (it == m_managers.end()) {
			LOG_FATAL("Manager for type not found");
		}
		return static_cast<Manager<T>&>(*it->second);
	}

  private:
	std::unordered_map<std::type_index, std::unique_ptr<IManager>> m_managers;
};
} // namespace Spark

#endif // SPARK_RESOURCE_HPP