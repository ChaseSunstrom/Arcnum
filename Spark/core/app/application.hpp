#ifndef SPARK_APPLICATION_HPP
#define SPARK_APPLICATION_HPP

#include <core/api.hpp>
#include <core/ecs/ecs.hpp>
#include <core/event/event_handler.hpp>
#include <core/pch.hpp>
#include <core/render/renderer.hpp>
#include <core/ecs/resource.hpp>
#include <core/system/manager.hpp>
#include <core/util/thread_pool.hpp>
#include <core/window/window.hpp>
#include <ranges>

#include "core/resource/asset.hpp"

namespace Spark {
	enum class SystemStage { First, PreUpdate, Update, PostUpdate, PreRender, Render, PostRender, Last, Count };

	struct SystemInfo {
		bool        threaded = false;
		bool        wait     = false;
		SystemStage stage    = SystemStage::Update;
	};

	class Application {
	  public:
		Application(GraphicsAPI gapi, f32 delta_time = 60.0f)
			: m_gapi(gapi)
			, m_delta_time(delta_time)
			, m_thread_pool(MakeUnique<ThreadPool>())
			, m_event_handler(MakeUnique<EventHandler>(*m_thread_pool))
			, m_ecs(MakeUnique<Ecs>(m_event_handler))
			, m_asset_manager(MakeUnique<AssetManager>())
			, m_running(false) {
			InitializeEventHandlers();
		}

		// System Management
		template<typename Func> Application& AddSystem(Func&& func, const SystemInfo& info = {}) {
			auto system = MakeUnique<System<std::decay_t<Func>>>(std::forward<Func>(func));
			m_systems[static_cast<size_t>(info.stage)].PushBack(Move(system));
			return *this;
		}

		// Asset Management
		template<typename T, typename... Args> T& CreateAsset(const String& name, Args&&... args) { return m_asset_manager->CreateAsset<T>(name, Forward<Args>(args)...); }

		template<typename T> T& GetAsset(const String& name) { return m_asset_manager->GetAsset<T>(name); }

		// Resource/Component access
		template<typename T> T& GetComponent(RefPtr<Entity> entity) { return m_ecs->GetComponent<T>(entity); }

		
        // Resource Management
		template<typename T> T& AddResource(T&& resource) {
			auto it = m_resources.Find(typeid(T));
			if (it == m_resources.End()) {
				auto container = MakeUnique<ResourceContainer<T>>();
				container->Set(Forward<T>(resource));
				auto& ref              = container->Get();
				m_resources[typeid(T)] = Move(container);
				return ref;
			}
			auto& container = static_cast<ResourceContainer<T>&>(*it->second);
			container.Set(Forward<T>(resource));
			return container.Get();
		}

		template<typename T> T& GetResource() {
			auto it = m_resources.Find(typeid(T));
			if (it == m_resources.End()) {
				LOG_FATAL("Resource not found: " + String(typeid(T).name()));
			}
			return static_cast<ResourceContainer<T>&>(*it->second).Get();
		}

		template<typename T> const T& GetResource() const {
			auto it = m_resources.Find(typeid(T));
			if (it == m_resources.End()) {
				LOG_FATAL("Resource not found: " + String(typeid(T).name()));
			}
			return static_cast<const ResourceContainer<T>&>(*it->second).Get();
		}

		// Window/Renderer setup
		template<IsWindow _Ty> Application& CreateWindow(const String& title, i32 width, i32 height) {
			m_window = MakeUnique<_Ty>(title, width, height, *m_event_handler);
			return *this;
		}

		template<IsRenderer _Ty> Application& CreateRenderer() {
			m_renderer = MakeUnique<_Ty>(m_gapi, m_window->GetFrameBuffer());
			return *this;
		}

		void Start() {
			if (!m_window || !m_renderer) {
				LOG_FATAL("Window and Renderer must be created before starting!");
			}

			m_running = true;

			// Start all systems
			for (auto& stage_systems : m_systems) {
				for (auto& system : stage_systems) {
					system->Start();
				}
			}

			MainLoop();
		}

		void Stop() { m_running = false; }
		bool IsRunning() const { return m_running; }

		// Core system access
		Ecs&          GetEcs() const { return *m_ecs; }
		Window&       GetWindow() const { return *m_window; }
		Renderer&     GetRenderer() const { return *m_renderer; }
		EventHandler& GetEventHandler() const { return *m_event_handler; }
		ThreadPool&   GetThreadPool() const { return *m_thread_pool; }

		f32  GetDeltaTime() const { return m_delta_time; }
		void SetDeltaTime(f32 delta_time) { m_delta_time = delta_time; }

	  private:
		// System management implementation
		template<typename Func> void AddRegularSystem(Func&& func, const SystemInfo& info) {
			auto system = MakeSystem([this, func = std::forward<Func>(func), info](Application& app, auto&&... args) {
				if (info.threaded) {
					m_thread_pool->Enqueue(TaskPriority::NORMAL, info.wait, [this, &func, &app, &args...] {
						std::unique_lock<std::mutex> lock(m_mutex);
						func(app, std::forward<decltype(args)>(args)...);
					});
				} else {
					func(app, std::forward<decltype(args)>(args)...);
				}
			});
			m_systems[static_cast<size_t>(info.stage)].PushBack(Move(system));
		}

		template<typename Func> void AddEventSystem(Func&& func, const SystemInfo& info) {
			auto wrapped_func = [this, func = std::forward<Func>(func), info](auto&&... events) {
				if (info.threaded) {
					m_thread_pool->Enqueue(TaskPriority::HIGH, info.wait, [this, &func, &events...] {
						std::unique_lock<std::mutex> lock(m_mutex);
						func(*this, std::forward<decltype(events)>(events)...);
					});
				} else {
					func(*this, std::forward<decltype(events)>(events)...);
				}
			};

			auto system = MakeEventSystem(*this, wrapped_func, m_event_handler);
			m_systems[static_cast<size_t>(info.stage)].PushBack(Move(system));
		}

		void MainLoop() {
			while (m_running && m_window->Running()) {
				for (size_t i = 0; i < static_cast<size_t>(SystemStage::Count); ++i) {
					RunSystemStage(static_cast<SystemStage>(i));
				}

				m_window->Update();
				m_thread_pool->SyncRegisteredTasks();
				m_ecs->ClearChangeTracker();
			}

			Cleanup();
		}

		void RunSystemStage(SystemStage stage) {
			auto& stage_systems = m_systems[static_cast<size_t>(stage)];
			for (auto& system : stage_systems) {
				system->Execute(*this);
			}
		}

		void Cleanup() {
			for (auto& stage_systems : m_systems | std::views::reverse) {
				for (auto& system : stage_systems) {
					system->Shutdown();
				}
			}
		}

		void InitializeEventHandlers() {
			m_event_handler->SubscribeToEvent<WindowClosedEvent>([this](const EventPtr<WindowClosedEvent>&) { Stop(); });
		}

		// Core systems
		UniquePtr<ThreadPool>   m_thread_pool;
		UniquePtr<EventHandler> m_event_handler;
		UniquePtr<Ecs>          m_ecs;
		UniquePtr<Window>       m_window;
		UniquePtr<Renderer>     m_renderer;
		UniquePtr<AssetManager> m_asset_manager;

		UnorderedMap<TypeIndex, UniquePtr<IResourceContainer>>                     m_resources;
		Array<Vector<UniquePtr<ISystem>>, static_cast<size_t>(SystemStage::Count)> m_systems;

		GraphicsAPI m_gapi;
		f32         m_delta_time;
		bool        m_running = false;
		std::mutex  m_mutex;
	};

	// Query parameter
	template<typename... Components> struct SystemParamTrait<Query<Components...>> {
		using Type = Query<Components...>;
		static Type Extract(Application& app) { return app.GetEcs().CreateQuery<Components...>(); }
	};

	// Asset parameter
	template<typename T>
		requires IsBaseOfV<Asset<T>, T>
	struct SystemParamTrait<T> {
		using Type = T&;
		static Type Extract(Application& app, const String& name) { return app.GetAsset<T>(name); }
	};

	// Parameter trait for asset access in systems
	template<typename T> struct SystemParamTrait<Asset<T>> {
		static_assert(IsBaseOfV<Asset<T>, T>, "Type must derive from Asset");
		using Type = const T&;
		static Type Extract(Application& app, const String& name) { return app.GetAsset<T>(name); }
	};

	 template<typename T> struct SystemParamTrait<ResMut<T>> {
		using Type = ResMut<T>;                   
		static Type Extract(Application& app) {
			return ResMut<T>(app.GetResource<T>());
		}
	};

	// And similarly for Res
	template<typename T> struct SystemParamTrait<Res<T>> {
		using Type = Res<T>;                  
		static Type Extract(Application& app) {
			return Res<T>(app.GetResource<T>());
		}
	};
} // namespace Spark

#endif