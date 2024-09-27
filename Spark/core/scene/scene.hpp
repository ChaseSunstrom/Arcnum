#ifndef SPARK_SCENE_HPP
#define SPARK_SCENE_HPP

#include <core/pch.hpp>
#include <core/render/model.hpp>
#include <core/system/manager.hpp>
#include "octree.hpp"

namespace Spark {
	class Scene {
	  public:
		friend class Manager<Scene>;
		~Scene() = default;

		void    OnEvent(const SharedPtr<ComponentEvent<TransformComponent>> event);
		void    Insert(const PointData& point_data);
		void    Update(const PointData& point_data);
		void    Remove(i64 entity_id);
		Octree& GetOctree() { return *m_octree; }

		// New methods for managing model instances
		void AddModelInstance(Entity& entity, const String& model_name, const _MATH Mat4& transform);
		void UpdateModelInstance(const String& model_name, size_t index, const _MATH Mat4& transform);
		void RemoveModelInstance(const String& model_name, size_t index);

		// Method to get all model instances
		const UnorderedMap<String, Vector<_MATH Mat4>>& GetModelInstances() const { return m_model_instances; }

	  private:
		Scene(const _MATH Vec3& center = _MATH Vec3(0), f32 width = 100)
			: m_octree(MakeUnique<Octree>(center, width)) {}

	  private:
		UniquePtr<Octree>                        m_octree;
		UnorderedMap<String, Vector<_MATH Mat4>> m_model_instances;
		UnorderedMap<i64, Pair<String, size_t>>  m_entity_to_instance_map;
	};

	// Deleted Copy functions
	template<> class Manager<Scene> : public IManager {
	  public:
		Manager()
			: m_registry(MakeUnique<Registry<Scene>>()) {
			// Default scene
			Create("Default Scene");
		}

		~Manager() = default;

		template<typename... Args> RefPtr<Scene> Create(const String& name, Args&&... args) {
			Scene* object = new Scene(Forward<Args>(args)...);
			Register(name, UniquePtr<Scene>(object));

			if (GetSize() == 1)
				m_current_scene = Get(name);

			return *m_current_scene;
		}

		void OnEvent(const SharedPtr<ComponentEvent<TransformComponent>> event) { m_current_scene->OnEvent(event); }

		RefPtr<Scene> Register(const String& name, UniquePtr<Scene> object) { return m_registry->Register(name, std::move(object)); }
		RefPtr<Scene> Get(const String& name) const { return m_registry->Get(name); }
		RefPtr<Scene> Get(const Handle handle) const { return m_registry->Get(handle); }
		RefPtr<Scene> GetCurrentScene() const { return m_current_scene; }

		RefPtr<Scene> SetCurrentScene(const String& name) {
			m_current_scene = m_registry->Get(name);
			return m_current_scene;
		}

		void Remove(const String& name) { m_registry->Remove(name); }

		void Remove(const Handle handle) { m_registry->Remove(handle); }

		size_t GetSize() const { return m_registry->GetSize(); }

		Vector<String> GetKeys() const { return m_registry->GetKeys(); }

		void SetRegistry(UniquePtr<Registry<Scene>> registry) { m_registry = Move(registry); }

		Registry<Scene>& GetRegistry() const { return *m_registry; }

	  private:
		RefPtr<Scene>              m_current_scene;
		UniquePtr<Registry<Scene>> m_registry;
	};
} // namespace Spark

#endif