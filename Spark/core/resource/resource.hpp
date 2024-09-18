#ifndef SPARK_RESOURCE_HPP
#define SPARK_RESOURCE_HPP

#include <core/pch.hpp>
#include <core/render/material.hpp>
#include <core/render/mesh.hpp>
#include <core/render/model.hpp>
#include <core/render/shader.hpp>
#include <core/scene/scene.hpp>
#include <core/render/camera.hpp>
#include <core/system/manager.hpp>

namespace Spark {

	class Application;

	// Dummy class for all resources, which includes, meshes, materials, models,
	// shaders, etc.
	class Resource {
	  public:
		virtual ~Resource() = default;
	};

	// Overload for Manager<Resource>
	template<> class Manager<Resource> {
	  public:
		friend class Application;
		Manager() {
			m_managers[typeid(StaticMesh)]   = std::make_unique<Manager<StaticMesh>>();
			m_managers[typeid(DynamicMesh)]  = std::make_unique<Manager<DynamicMesh>>();
			m_managers[typeid(Material)]     = std::make_unique<Manager<Material>>();
			m_managers[typeid(StaticModel)]  = std::make_unique<Manager<StaticModel>>();
			m_managers[typeid(DynamicModel)] = std::make_unique<Manager<DynamicModel>>();
			m_managers[typeid(RenderShader)] = std::make_unique<Manager<RenderShader>>();
			m_managers[typeid(Scene)]        = std::make_unique<Manager<Scene>>();
			m_managers[typeid(Camera)]       = std::make_unique<Manager<Camera>>();
		}

		~Manager() = default;

		template<typename _Ty, typename... Args> _Ty& Create(const std::string& name, Args&&... args) {
			auto& manager = GetManager<_Ty>();
			return manager.Create(name, std::forward<Args>(args)...);
		}

		template<typename _Ty> _Ty& Register(const std::string& name, std::unique_ptr<_Ty> object) {
			auto& manager = GetManager<_Ty>();
			return manager.Register(name, std::move(object));
		}

		template<typename _Ty> _Ty& Get(const std::string& name) {
			auto& manager = GetManager<_Ty>();
			return manager.Get(name);
		}

		template<typename _Ty> _Ty& Get(const Handle handle) {
			auto& manager = GetManager<_Ty>();
			return manager.Get(handle);
		}

		template<typename _Ty> _Ty GetCopy(const std::string& name) {
			auto& manager = GetManager<_Ty>();
			return manager.GetCopy(name);
		}

		template<typename _Ty> _Ty GetCopy(const Handle handle) {
			auto& manager = GetManager<_Ty>();
			return manager.GetCopy(handle);
		}

		template<typename _Ty> void Remove(const std::string& name) {
			auto& manager = GetManager<_Ty>();
			manager.Remove(name);
		}

		template<typename _Ty> void Remove(const Handle handle) {
			auto& manager = GetManager<_Ty>();
			manager.Remove(handle);
		}

		template<typename _Ty> std::vector<std::string> GetKeys() {
			auto& manager = GetManager<_Ty>();
			return manager.GetKeys();
		}

		template<typename _Ty> void SetRegistry(std::unique_ptr<Registry<_Ty>> registry) {
			auto& manager = GetManager<_Ty>();
			manager.SetRegistry(std::move(registry));
		}

		template<typename _Ty> Manager<_Ty>& GetManager() {
			auto it = m_managers.find(typeid(_Ty));
			if (it == m_managers.end()) {
				LOG_FATAL("Manager for type not found");
			}
			return static_cast<Manager<_Ty>&>(*it->second);
		}

	  private:
		std::unordered_map<std::type_index, std::unique_ptr<IManager>> m_managers;
		std::mutex                                                     m_mutex;
	};
} // namespace Spark

#endif // SPARK_RESOURCE_HPP