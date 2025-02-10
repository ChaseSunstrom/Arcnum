#ifndef SPARK_APPLICATION_HPP
#define SPARK_APPLICATION_HPP

#include "spark_pch.hpp"
#include "spark_ecs.hpp"
#include "spark_window.hpp"
#include "spark_event.hpp"
#include "spark_event_queue.hpp"
#include "spark_threading.hpp"
#include "spark_stopwatch.hpp"
#include "spark_graphics_api.hpp"
#include "spark_defer.hpp"
#include "spark_layer_stack.hpp"
#include "spark_renderer_layer.hpp"
#include "spark_command_queue.hpp"
#include "spark_delta_time.hpp"
#include "spark_event_layer.hpp"
#include "special/spark_modding.hpp"
#include "spark_item.hpp"

namespace spark
{
	template <typename T>
	struct IsSparkQuery : std::false_type {};

	template <typename... Ts>
	struct IsSparkQuery<Query<Ts...>> : std::true_type {};

	template <typename T>
	struct IsEvent : std::false_type {};

	// If you have a multi-event type
	template <typename... Ts>
	struct IsEvent<Event<Ts...>> : std::true_type {};

	// ParamTrait for function arguments
	template <typename T>
	struct ParamTrait
	{
		using decayed = std::remove_cv_t<std::remove_reference_t<T>>;
		static constexpr bool is_query = IsSparkQuery<decayed>::value;
		static constexpr bool is_event = IsEvent<decayed>::value;
		static constexpr bool is_resource = (!is_query && !is_event);
	};

	// If user wants to build SparkQuery<Ts...> from a coordinator
	template <typename T>
	struct SparkQueryComponents; // specialized for SparkQuery

	template <typename... Ts>
	struct SparkQueryComponents<Query<Ts...>>
	{
		static Query<Ts...> Get(Coordinator& coord)
		{
			return coord.CreateQuery<Ts...>();
		}
	};

	enum class SystemPhase
	{
		BEFORE_START,
		ON_START,
		BEFORE_UPDATE,
		UPDATE,
		AFTER_UPDATE,
		ON_SHUTDOWN
	};

	enum class SystemExecutionMode
	{
		SINGLE_THREADED,
		MULTITHREADED_SYNC,
		MULTITHREADED_ASYNC
	};

	using SystemPriority = threading::TaskPriority;

	struct SystemSettings
	{
		SystemPhase phase = SystemPhase::UPDATE;
		SystemExecutionMode execution_mode = SystemExecutionMode::SINGLE_THREADED;
		SystemPriority priority = SystemPriority::NORMAL;
	};

	template <typename R, typename... Args>
	struct FunctionTraitsImpl
	{
		using return_type = R;
		using args_tuple = std::tuple<Args...>;
		static constexpr size_t arity = sizeof...(Args);

		template <size_t Index>
		using arg = std::tuple_element_t<Index, args_tuple>;
	};

	template <typename T, typename = void>
	struct FunctionTraits;

	// For free functions or static member functions
	template <typename R, typename... Args>
	struct FunctionTraits<R(Args...), void> : FunctionTraitsImpl<R, Args...> {};

	// For function pointers
	template <typename R, typename... Args>
	struct FunctionTraits<R(*)(Args...), void> : FunctionTraitsImpl<R, Args...> {};

	// For class member functions
	template <typename C, typename R, typename... Args>
	struct FunctionTraits<R(C::*)(Args...), void> : FunctionTraitsImpl<R, Args...> {};

	template <typename C, typename R, typename... Args>
	struct FunctionTraits<R(C::*)(Args...) const, void> : FunctionTraitsImpl<R, Args...> {};

	// For functors with operator()
	template <typename T>
	struct FunctionTraits<T, std::void_t<decltype(&T::operator())>>
		: FunctionTraits<decltype(&T::operator())> {};

	// For a single-type event: Event<T>
	template <typename EventType>
	struct EventTypeExtractor;

	template <typename T>
	struct EventTypeExtractor<Event<T>>
	{
		static std::vector<std::type_index> Get()
		{
			return { std::type_index(typeid(T)) };
		}
	};

	// For a multi-type event: Event<T, U, More...>
	template <typename T, typename U, typename... More>
	struct EventTypeExtractor<Event<T, U, More...>>
	{
		static std::vector<std::type_index> Get()
		{
			std::vector<std::type_index> types = { std::type_index(typeid(T)), std::type_index(typeid(U)) };
			(types.push_back(std::type_index(typeid(More))), ...);
			return types;
		}
	};

	class Application;

	// For convenience
	template <typename T>
	using Ref = T&;

	template <typename T>
	using CRef = const T&;

	namespace detail
	{
		struct ISystem
		{
			virtual ~ISystem() = default;
			virtual void Execute(Application& app) = 0;
			virtual void Execute(Application& app, std::shared_ptr<IEvent> forced_event) = 0;
			virtual SystemPriority GetPriority() const = 0;
			virtual SystemExecutionMode GetExecutionMode() const = 0;
		};

		template <typename T, bool is_resource>
		struct ArgHolder;

		// For non-resource types
		template <typename T>
		struct ArgHolder<T, false>
		{
			T m_value;
			explicit ArgHolder(T&& value)
				: m_value(std::move(value))
			{
			}
			T Get()
			{
				return m_value;
			}
		};

		// For resource types by value
		template <typename T>
		struct ArgHolder<T, true>
		{
			using resource_type = std::remove_reference_t<T>;
			using lock_type = std::conditional_t<
				std::is_const_v<resource_type>,
				threading::LockedCRef<std::remove_const_t<resource_type>>,
				threading::LockedRef<resource_type>
			>;
			std::shared_ptr<lock_type> m_lock;
			explicit ArgHolder(std::shared_ptr<lock_type> lock)
				: m_lock(std::move(lock))
			{
			}
			T Get()
			{
				return m_lock->Get();
			}
		};

		// For resource types by reference
		template <typename U>
		struct ArgHolder<U&, true>
		{
			using resource_type = std::remove_reference_t<U>;
			using lock_type = std::conditional_t<
				std::is_const_v<resource_type>,
				threading::LockedCRef<std::remove_const_t<resource_type>>,
				threading::LockedRef<resource_type>
			>;
			std::shared_ptr<lock_type> m_lock;
			explicit ArgHolder(std::shared_ptr<lock_type> lock)
				: m_lock(std::move(lock))
			{
			}
			U& Get()
			{
				return m_lock->Get();
			}
		};

		struct ResourceAggregator
		{
			std::map<std::type_index, std::any> resource_map;
			bool coordinator_needed = false;
			std::any coordinator_any; // shared_ptr<LockedRef<Coordinator>>
		};

		template <typename... Ts, typename T>
		bool SetVariant(Event<Ts...>& evt, T* typed_ptr)
		{
			if (typed_ptr == nullptr)
			{
				return false;
			}
			std::shared_ptr<T> sp = std::make_shared<T>(*typed_ptr);
			evt = Event<Ts...>(sp);
			return true;
		}

		template <typename... Ts>
		bool AssignVariantToEvent(Event<Ts...>& evt, std::type_index tid, void* raw_ptr)
		{
			bool assigned = false;
			((tid == std::type_index(typeid(Ts))
				? (assigned = SetVariant(evt, reinterpret_cast<Ts*>(raw_ptr)))
				: false) || ...);
			return assigned;
		}

		template <typename t_expected>
		std::shared_ptr<t_expected> ConvertForcedEvent(std::shared_ptr<IEvent> forced)
		{
			auto casted = std::dynamic_pointer_cast<t_expected>(forced);
			if (casted)
			{
				return casted;
			}
			t_expected temp_evt;
			bool assigned = false;
			forced->VisitActive([&](std::type_index tid, void* raw_ptr)
				{
					assigned = AssignVariantToEvent(temp_evt, tid, raw_ptr);
				});
			if (!assigned)
			{
				return nullptr;
			}
			return std::make_shared<t_expected>(std::move(temp_evt));
		}

		template <typename Arg>
		void MarkNeededResources(ResourceAggregator& agg)
		{
			if constexpr (ParamTrait<Arg>::is_event)
			{
				// No resource to lock for events.
			}
			else if constexpr (ParamTrait<Arg>::is_query)
			{
				agg.coordinator_needed = true;
			}
			else if constexpr (ParamTrait<Arg>::is_resource)
			{
				using decayed = std::remove_reference_t<Arg>;
				agg.resource_map[std::type_index(typeid(decayed))] = std::any{};
			}
		}

		template <typename T>
		threading::AutoLockable<T>* GetResourceInternal(std::unordered_map<std::type_index, std::any>& resources)
		{
			using pointer_type = std::remove_reference_t<T>;
			auto it = resources.find(std::type_index(typeid(pointer_type)));
			if (it == resources.end())
			{
				throw std::runtime_error("Resource not found for type: " + std::string(typeid(T).name()));
			}
			auto ptr_any = std::any_cast<std::shared_ptr<threading::AutoLockable<pointer_type>>>(it->second);
			if (!ptr_any)
			{
				throw std::runtime_error("any_cast failed for " + std::string(typeid(T).name()));
			}
			return *ptr_any;
		}

		// LockResources uses GetResourceInternal to lock each resource.
		void LockResources(Application& app, ResourceAggregator& agg);

		template <typename Arg>
		auto BuildParam(Application& app, ResourceAggregator& agg, std::shared_ptr<IEvent> forced)
		{
			using decayed = std::remove_reference_t<Arg>;
			if constexpr (ParamTrait<Arg>::is_event)
			{
				if (!forced)
				{
					throw std::runtime_error("Event param but no forced event!");
				}
				auto casted = ConvertForcedEvent<decayed>(forced);
				if (!casted)
				{
					throw std::runtime_error("Could not convert forced event param to expected type!");
				}
				return ArgHolder<Arg, false>(std::move(*casted));
			}
			else if constexpr (ParamTrait<Arg>::is_query)
			{
				if (!agg.coordinator_needed)
					throw std::runtime_error("Query param but aggregator says no coordinator??");
				using lock_ptr = std::shared_ptr<threading::LockedRef<Coordinator>>;
				auto sp_co = std::any_cast<lock_ptr>(&agg.coordinator_any);
				if (!sp_co || !(*sp_co))
					throw std::runtime_error("No coordinator locked for query param!");
				auto& locked_co = **sp_co;
				auto& real_co = locked_co.Get();
				auto query_obj = SparkQueryComponents<decayed>::Get(real_co);
				struct QueryArgHolder
				{
					lock_ptr co_locked;
					decayed query_obj;
					QueryArgHolder(lock_ptr co, decayed&& q_obj)
						: co_locked(std::move(co)), query_obj(std::move(q_obj))
					{
					}
					decayed Get() { return query_obj; }
				};
				return QueryArgHolder(*sp_co, std::move(query_obj));
			}
			else if constexpr (ParamTrait<Arg>::is_resource)
			{
				auto ti = std::type_index(typeid(decayed));
				auto it = agg.resource_map.find(ti);
				if (it == agg.resource_map.end())
					throw std::runtime_error("No aggregator entry for resource => " + std::string(typeid(Arg).name()));
				if constexpr (std::is_reference_v<Arg>)
				{
					using lock_type = std::conditional_t<
						std::is_const_v<decayed>,
						threading::LockedCRef<std::remove_const_t<decayed>>,
						threading::LockedRef<decayed>
					>;
					using sp_type = std::shared_ptr<lock_type>;
					auto sp = std::any_cast<sp_type>(&it->second);
					if (!sp || !(*sp))
						throw std::runtime_error("any_cast to shared_ptr<LockedRef<T>> failed for " + std::string(typeid(Arg).name()));
					return ArgHolder<Arg, true>(*sp);
				}
				else
				{
					using lock_type = std::conditional_t<
						std::is_const_v<decayed>,
						threading::LockedCRef<std::remove_const_t<decayed>>,
						threading::LockedRef<decayed>
					>;
					using sp_type = std::shared_ptr<lock_type>;
					auto sp = std::any_cast<sp_type>(&it->second);
					if (!sp || !(*sp))
						throw std::runtime_error("any_cast to shared_ptr<LockedRef<T>> failed for " + std::string(typeid(Arg).name()));
					return ArgHolder<Arg, true>(*sp);
				}
			}
			else
			{
				static_assert(sizeof(Arg) == 0, "Unhandled aggregator param type!");
			}
		}

		template <typename Func>
		struct System : public ISystem
		{
			Func m_func;
			SystemExecutionMode m_mode;
			SystemPriority m_priority;
			std::vector<std::type_index> m_expected_event_types;

			System(Func&& func, const SystemSettings& settings)
				: m_func(std::forward<Func>(func))
				, m_mode(settings.execution_mode)
				, m_priority(settings.priority)
			{
				using traits = FunctionTraits<Func>;
				constexpr size_t n = traits::arity;
				ForSequence(std::make_index_sequence<n>{}, [this](auto i)
					{
						using arg_t = typename traits::template arg<i.value>;
						if constexpr (ParamTrait<arg_t>::is_event)
						{
							using decayed = std::remove_cv_t<std::remove_reference_t<arg_t>>;
							auto types = EventTypeExtractor<decayed>::Get();
							m_expected_event_types.insert(m_expected_event_types.end(), types.begin(), types.end());
						}
					});
			}

			void Execute(Application& app) override
			{
				ExecuteImpl(app, nullptr);
			}

			void Execute(Application& app, std::shared_ptr<IEvent> forced) override
			{
				if (!m_expected_event_types.empty() && forced)
				{
					bool match = false;
					forced->VisitActive([&](std::type_index tid, void*)
						{
							for (auto& expected : m_expected_event_types)
							{
								if (expected == tid)
								{
									match = true;
									break;
								}
							}
						});
					if (!match)
					{
						return;
					}
				}

				ExecuteImpl(app, forced);
			}

			SystemPriority GetPriority() const override { return m_priority; }
			SystemExecutionMode GetExecutionMode() const override { return m_mode; }

		private:
			void ExecuteImpl(Application& app, std::shared_ptr<IEvent> forced)
			{
				CallFunc(app, forced);
			}

			template <size_t... Is, typename F>
			static void ForSequence(std::index_sequence<Is...>, F&& fn)
			{
				(fn(std::integral_constant<size_t, Is>{}), ...);
			}

			void CallFunc(Application& app, std::shared_ptr<IEvent> forced)
			{
				using traits = FunctionTraits<Func>;
				constexpr size_t n = traits::arity;
				ResourceAggregator agg;
				ForSequence(std::make_index_sequence<n>{}, [&](auto i)
					{
						using arg_t = typename traits::template arg<i.value>;
						MarkNeededResources<arg_t>(agg);
					});
				LockResources(app, agg);
				auto holders = BuildHolders<typename traits::args_tuple>(
					app, agg, forced, std::make_index_sequence<n>{}
				);
				std::apply([this](auto & ... holder)
					{
						m_func(holder.Get()...);
					}, holders);
			}

			template <typename args_tuple, size_t... Is>
			auto BuildHolders(Application& app, ResourceAggregator& agg,
				std::shared_ptr<IEvent> forced,
				std::index_sequence<Is...>)
			{
				return std::make_tuple(
					BuildParam<typename std::tuple_element<Is, args_tuple>::type>(app, agg, forced)...
				);
			}
		};
	} // namespace detail


	class Application
	{
	public:
		Application(GraphicsApi gapi,
			const std::string& title,
			int32_t win_width,
			int32_t win_height,
			bool vsync = false)
			: m_gapi(gapi)
			, m_mod_manager(this)
		{
			// Basic layering
			m_layer_stack.PushLayer<WindowLayer>(m_event_queue, gapi, title, win_width, win_height, vsync);
			m_layer_stack.PushLayer<RendererLayer>(gapi, m_command_queue);
			m_layer_stack.PushLayer<EventLayer>(*this, m_event_queue);

			// Add these resources
			AddResource(*this);
			AddResource(m_coordinator);
			AddResource(m_thread_pool);
			AddResource(m_delta_time);
			AddResource(m_gapi);
			AddResource(m_layer_stack);
			AddResource(m_command_queue);
			AddResource(m_item_manager);
			AddResource(m_event_queue);
		}

		~Application()
		{
			Close();
		}

		Application& AddThreads(std::size_t num_threads)
		{
			m_thread_pool.AddThreads(num_threads);
			return *this;
		}

		Application& Start()
		{
			DispatchSystemsForPhase(SystemPhase::BEFORE_START);
			m_layer_stack.Start();
			DispatchSystemsForPhase(SystemPhase::ON_START);
			return *this;
		}

		Application& Run()
		{
			while (IsRunning())
			{
				DispatchSystemsForPhase(SystemPhase::BEFORE_UPDATE);
				m_layer_stack.Update(m_delta_time);
				DispatchSystemsForPhase(SystemPhase::UPDATE);
				DispatchSystemsForPhase(SystemPhase::AFTER_UPDATE);
			}
			return *this;
		}

		Application& Close()
		{
			m_thread_pool.Shutdown();
			DispatchSystemsForPhase(SystemPhase::ON_SHUTDOWN);
			m_layer_stack.Stop();
			return *this;
		}

		bool IsRunning()
		{
			return GetWindowLayer().Running();
		}

		Application& LoadMod(const std::string& mod_path)
		{
			auto handle = m_mod_manager.LoadLibrary(mod_path);
			if (!handle)
				return *this;
			auto mod_instance = m_mod_manager.LoadModInstance(handle);
			if (!mod_instance)
			{
				m_mod_manager.UnloadLibrary(handle);
			}
			return *this;
		}

		template <typename... Funcs>
		Application& RegisterSystems(Funcs &&... funcs)
		{
			static_assert(sizeof...(funcs) > 0, "RegisterSystems requires at least one function.");
			using last_type = std::decay_t<std::tuple_element_t<sizeof...(funcs) - 1, std::tuple<Funcs...>>>;
			constexpr bool last_is_settings = std::is_same_v<last_type, SystemSettings>;
			if constexpr (last_is_settings)
			{
				constexpr size_t n = sizeof...(funcs) - 1;
				auto args_tuple = std::forward_as_tuple(std::forward<Funcs>(funcs)...);
				const SystemSettings& settings = std::get<n>(args_tuple);
				RegisterSystemsImpl(args_tuple, std::make_index_sequence<n>{}, settings);
			}
			else
			{
				RegisterSystems(std::forward<Funcs>(funcs)..., SystemSettings{});
			}
			return *this;
		}

		template <typename Tuple, size_t... Is>
		Application& RegisterSystemsImpl(Tuple&& tup, std::index_sequence<Is...>, const SystemSettings& settings)
		{
			(RegisterSystem(std::get<Is>(tup), settings), ...);
			return *this;
		}

		template <typename Func>
		Application& RegisterSystem(Func&& system_func, const SystemSettings& settings = {})
		{
			using f = std::decay_t<Func>;
			using traits = FunctionTraits<f>;
			constexpr size_t arity = traits::arity;
			bool has_event = false;
			ForSequence(std::make_index_sequence<arity>{}, [&](auto i)
				{
					using arg_t = typename traits::template arg<i.value>;
					if constexpr (ParamTrait<arg_t>::is_event)
					{
						has_event = true;
					}
				});
			auto system_obj = std::make_unique<detail::System<f>>(std::forward<Func>(system_func), settings);
			if (has_event)
			{
				m_event_systems.emplace_back(std::move(system_obj));
			}
			else
			{
				m_systems[settings.phase].emplace_back(std::move(system_obj));
			}
			return *this;
		}

		template <typename... Ts>
		Application& SubmitEvent(const Event<Ts...>& ev)
		{
			m_event_queue.SubmitEvent<Ts...>(ev);
			return *this;
		}

		template <typename T>
		Application& SubmitEvent(const T& ev)
		{
			Event<T> single(ev);
			m_event_queue.SubmitEvent(single);
			return *this;
		}

		// Resource management
		template <typename T>
		bool HasResource() const
		{
			return (m_resources.find(std::type_index(typeid(T))) != m_resources.end());
		}

		template <typename T>
		Application& AddResource(T& obj)
		{
			// Always store the resource as a pointer (so that AutoLockable<T*> is used).
			m_resources[std::type_index(typeid(T))] =
				std::make_shared<threading::AutoLockable<T*>>(&obj);
			return *this;
		}

		template <typename T>
		threading::AutoLockable<T*>* GetResource()
		{
			auto it = m_resources.find(std::type_index(typeid(T)));
			if (it == m_resources.end())
			{
				throw std::runtime_error("Resource not found for type: " + std::string(typeid(T).name()));
			}
			auto ptr_any = std::any_cast<std::shared_ptr<threading::AutoLockable<T*>>>(it->second);
			if (!ptr_any)
			{
				throw std::runtime_error("any_cast failed for " + std::string(typeid(T).name()));
			}
			return ptr_any.get();
		}


		Application& SetDeltaTime(DeltaTime<double> dt)
		{
			m_delta_time = dt;
			return *this;
		}

		Coordinator& GetCoordinator()
		{
			return m_coordinator;
		}

		threading::ThreadPool& GetThreadPool()
		{
			return m_thread_pool;
		}

		void DispatchSystemsForPhase(SystemPhase phase)
		{
			auto it = m_systems.find(phase);
			if (it == m_systems.end())
				return;
			std::vector<std::future<void>> sync_tasks;
			for (auto& sys_ptr : it->second)
			{
				switch (sys_ptr->GetExecutionMode())
				{
				case SystemExecutionMode::SINGLE_THREADED:
					sys_ptr->Execute(*this);
					break;
				case SystemExecutionMode::MULTITHREADED_SYNC:
				{
					auto task = m_thread_pool.Enqueue(
						sys_ptr->GetPriority(),
						[this, &sys_ptr]()
						{
							sys_ptr->Execute(*this);
						}
					);
					sync_tasks.emplace_back(std::move(task.result));
					break;
				}
				case SystemExecutionMode::MULTITHREADED_ASYNC:
					m_thread_pool.Enqueue(
						sys_ptr->GetPriority(),
						[this, &sys_ptr]()
						{
							sys_ptr->Execute(*this);
						}
					);
					break;
				}
			}
			for (auto& f : sync_tasks)
			{
				f.get();
			}
		}

		IRenderer& GetRenderer()
		{
			return GetRendererLayer().GetRenderer();
		}

		Application& ChangeGraphicsApi(GraphicsApi new_gapi)
		{
			if (new_gapi == m_gapi)
				return *this;
			GetWindowLayer().SetGraphicsApi(new_gapi);
			GetRendererLayer().SetGraphicsApi(new_gapi);
			return *this;
		}

		std::vector<std::unique_ptr<detail::ISystem>>& GetEventSystems()
		{
			return m_event_systems;
		}

		// Adds an item by copying the provided resource.
		template <typename T>
		T& AddItem(const std::string& key, const T& res)
		{
			return m_item_manager.AddItem(key, res);
		}

		// Adds an item by constructing T in place using perfect forwarding.
		template <typename T, typename... Args>
		T& AddItem(const std::string& key, Args&&... args)
		{
			return m_item_manager.AddItem<T>(key, std::forward<Args>(args)...);
		}

		// Retrieves a mutable reference to an item. Throws std::runtime_error if not found.
		template <typename T>
		T& GetItem(const std::string& key)
		{
			return m_item_manager.GetItem<T>(key);
		}

		// Retrieves a const reference to an item. Throws std::runtime_error if not found.
		template <typename T>
		const T& GetItem(const std::string& key) const
		{
			return m_item_manager.GetItem<T>(key);
		}

		// Checks whether an item of type T with the specified key exists.
		template <typename T>
		bool HasItem(const std::string& key) const
		{
			return m_item_manager.HasItem<T>(key);
		}

		template <ValidCommand T>
		void SubmitCommand(const T& command)
		{
			m_command_queue.SubmitCommand<T>(command);
		}

		template <ValidCommand T, typename... Args>
		void SubmitCommand(Args&&... args)
		{
			m_command_queue.SubmitCommand<T>(std::forward<Args>(args)...);
		}

		// Removes the item of type T with the specified key.
		template <typename T>
		void RemoveItem(const std::string& key)
		{
			m_item_manager.RemoveItem<T>(key);
		}

		// Clears all items from the manager.
		void ClearItems()
		{
			m_item_manager.Clear();
		}

	private:
		template <size_t... Is, typename F>
		static void ForSequence(std::index_sequence<Is...>, F&& func)
		{
			(func(std::integral_constant<size_t, Is>{}), ...);
		}

		WindowLayer& GetWindowLayer()
		{
			return *m_layer_stack.GetLayer<WindowLayer>();
		}

		RendererLayer& GetRendererLayer()
		{
			return *m_layer_stack.GetLayer<RendererLayer>();
		}

		EventLayer& GetEventLayer()
		{
			return *m_layer_stack.GetLayer<EventLayer>();
		}

	private:
		GraphicsApi m_gapi;
		ModManager m_mod_manager;
		LayerStack m_layer_stack;
		CommandQueue m_command_queue;
		EventQueue m_event_queue;
		ItemManager m_item_manager;
		threading::ThreadPool m_thread_pool;
		DeltaTime<f64> m_delta_time;
		Coordinator m_coordinator;
		std::unordered_map<SystemPhase, std::vector<std::unique_ptr<detail::ISystem>>> m_systems;
		std::vector<std::unique_ptr<detail::ISystem>> m_event_systems;
		std::unordered_map<std::type_index, std::any> m_resources;

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(Application&&) = delete;
	};

	namespace detail
	{
		inline void LockResources(Application& app, ResourceAggregator& agg)
		{
			if (agg.coordinator_needed)
			{
				auto coord_lockable = app.GetResource<Coordinator>();
				auto locked_obj = coord_lockable->Lock();
				using lock_type = threading::LockedRef<Coordinator>;
				auto sp = std::make_shared<lock_type>(std::move(locked_obj));
				agg.coordinator_any = std::move(sp);
			}
			std::vector<std::type_index> types;
			for (auto& kv : agg.resource_map)
			{
				types.push_back(kv.first);
			}
			std::sort(types.begin(), types.end(), [](auto& a, auto& b) { return a < b; });
			for (auto& ti : types)
			{
				if (ti == std::type_index(typeid(Coordinator)))
				{
					if (agg.coordinator_needed)
					{
						agg.resource_map[ti] = agg.coordinator_any;
					}
					else
					{
						auto coord_lockable = app.GetResource<Coordinator>();
						auto locked_obj = coord_lockable->Lock();
						using lock_type = threading::LockedRef<Coordinator>;
						auto sp = std::make_shared<lock_type>(std::move(locked_obj));
						agg.resource_map[ti] = std::move(sp);
					}
				}
				else if (ti == std::type_index(typeid(Application)))
				{
					auto app_lockable = app.GetResource<Application>();
					auto locked_obj = app_lockable->Lock();
					using lock_type = threading::LockedRef<Application>;
					auto sp = std::make_shared<lock_type>(std::move(locked_obj));
					agg.resource_map[ti] = std::move(sp);
				}
				else if (ti == std::type_index(typeid(threading::ThreadPool)))
				{
					auto pool_lockable = app.GetResource<threading::ThreadPool>();
					auto locked_obj = pool_lockable->Lock();
					using lock_type = threading::LockedRef<threading::ThreadPool>;
					auto sp = std::make_shared<lock_type>(std::move(locked_obj));
					agg.resource_map[ti] = std::move(sp);
				}
				else
				{
					auto s = ti.name();
					throw std::runtime_error("Unknown resource type => " + std::string(s));
				}
			}
		}

	}
} // namespace spark

#endif // SPARK_APPLICATION_HPP
