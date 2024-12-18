#ifndef SPARK_RESOURCE_HPP
#define SPARK_RESOURCE_HPP

#include <core/pch.hpp>
#include <core/render/material.hpp>
#include <core/render/mesh.hpp>
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
			m_managers[typeid(GenericMesh)]  = MakeUnique<Manager<GenericMesh>>();
			m_managers[typeid(Material)]     = MakeUnique<Manager<Material>>();
			m_managers[typeid(Shader)]       = MakeUnique<Manager<Shader>>();
			m_managers[typeid(Scene)]        = MakeUnique<Manager<Scene>>();
			m_managers[typeid(Camera)]       = MakeUnique<Manager<Camera>>();
		}

		~Manager() = default;

		template<typename _Ty, typename... Args> _Ty& Create(const String& name, Args&&... args) {
			auto& manager = GetManager<_Ty>();
			return manager.Create(name, Forward<Args>(args)...);
		}

		template<typename _Ty> _Ty& Register(const String& name, UniquePtr<_Ty> object) {
			auto& manager = GetManager<_Ty>();
			return manager.Register(name, Move(object));
		}

		template<typename _Ty> _Ty& Get(const String& name) {
			auto& manager = GetManager<_Ty>();
			return manager.Get(name);
		}

		template<typename _Ty> _Ty& Get(const Handle handle) {
			auto& manager = GetManager<_Ty>();
			return manager.Get(handle);
		}

		template<typename _Ty> _Ty GetCopy(const String& name) {
			auto& manager = GetManager<_Ty>();
			return manager.GetCopy(name);
		}

		template<typename _Ty> _Ty GetCopy(const Handle handle) {
			auto& manager = GetManager<_Ty>();
			return manager.GetCopy(handle);
		}

		template<typename _Ty> void Remove(const String& name) {
			auto& manager = GetManager<_Ty>();
			manager.Remove(name);
		}

		template<typename _Ty> void Remove(const Handle handle) {
			auto& manager = GetManager<_Ty>();
			manager.Remove(handle);
		}

		template<typename _Ty> Vector<String> GetKeys() {
			auto& manager = GetManager<_Ty>();
			return manager.GetKeys();
		}

		template<typename _Ty> void SetRegistry(UniquePtr<Registry<_Ty>> registry) {
			auto& manager = GetManager<_Ty>();
			manager.SetRegistry(Move(registry));
		}

		template<typename _Ty> Manager<_Ty>& GetManager() {
			auto it = m_managers.Find(typeid(_Ty));
			if (it == m_managers.End()) {
				LOG_FATAL("Manager for type not found");
			}
			return static_cast<Manager<_Ty>&>(*it->second);
		}

	  private:
		UnorderedMap<TypeIndex, UniquePtr<IManager>> m_managers;
		std::mutex                                                     m_mutex;
	};
} // namespace Spark

#endif // SPARK_RESOURCE_HPP