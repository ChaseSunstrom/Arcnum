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

#include <any>
#include <typeindex>
#include <memory>
#include <unordered_map>
#include <stdexcept>
#include <mutex>
#include <shared_mutex>
#include <tuple>
#include <utility>

namespace spark
{
	// --------------------------------------------------------------------
	// In order to automatically lock resources for the entire system call,
	// we define a helper in the detail namespace that creates an "argument
	// holder" for each system parameter.
	//
	// For resource parameters (as determined by ParamTrait), we immediately lock
	// the resource (via Lock() or CLock()) and store the resulting RAII lock in an
	// ArgHolder. Its Get() method returns the underlying reference.
	//
	// For non-resource types, the ArgHolder simply holds the computed value.
	// --------------------------------------------------------------------
	namespace detail
	{
		template <typename T, bool IsResource>
		struct ArgHolder;

		// For resource types
		template <typename T>
		struct ArgHolder<T, true>
		{
			// T is expected to be a reference type (e.g. Coordinator&).
			// Remove the reference to get the underlying resource type.
			using ResourceType = std::remove_reference_t<T>;

			// If ResourceType is const, use LockedCRef; otherwise, LockedRef.
			std::conditional_t<std::is_const_v<ResourceType>,
				threading::LockedCRef<std::remove_const_t<ResourceType>>,
				threading::LockedRef<ResourceType>> m_lock;

			explicit ArgHolder(std::conditional_t<std::is_const_v<ResourceType>,
				threading::LockedCRef<std::remove_const_t<ResourceType>>,
				threading::LockedRef<ResourceType>> lock)
				: m_lock(std::move(lock))
			{
			}

			// Get() returns the underlying resource reference.
			T Get()
			{
				return m_lock.Get();
			}
		};

		// For non-resource types
		template <typename T>
		struct ArgHolder<T, false>
		{
			T m_value;
			explicit ArgHolder(T&& value)
				: m_value(std::forward<T>(value))
			{
			}
			T Get()
			{
				return m_value;
			}
		};

		// MakeArgHolder creates an ArgHolder for a given system parameter type.
		// For resources it locks the resource; for others it defers to ComputeArgument.
		template <typename Arg>
		auto MakeArgHolder(Application& app, std::shared_ptr<IEvent> forced_evt);

		// CallFuncWithArgHolders unpacks a tuple of argument holders and calls f.
		template <typename Func, typename Tuple>
		void CallFuncWithArgHolders(Func& f, Tuple& holders)
		{
			std::apply([&](auto& ... holder) { f(holder.Get()...); }, holders);
		}
	} // namespace detail

	enum class SystemPhase
	{
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

	// FunctionTraits for analyzing system function signatures
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

	template <typename R, typename... Args>
	struct FunctionTraits<R(Args...), void> : FunctionTraitsImpl<R, Args...> {};

	template <typename R, typename... Args>
	struct FunctionTraits<R(*)(Args...), void> : FunctionTraitsImpl<R, Args...> {};

	template <typename C, typename R, typename... Args>
	struct FunctionTraits<R(C::*)(Args...), void> : FunctionTraitsImpl<R, Args...> {};

	template <typename C, typename R, typename... Args>
	struct FunctionTraits<R(C::*)(Args...) const, void> : FunctionTraitsImpl<R, Args...> {};

	template <typename T>
	struct FunctionTraits<T, std::void_t<decltype(&T::operator())>>
		: FunctionTraits<decltype(&T::operator())>
	{
	};

	template <typename T>
	struct IsSparkQuery : std::false_type {};

	template <typename... Ts>
	struct IsSparkQuery<Query<Ts...>> : std::true_type {};

	template <typename T>
	struct IsEvent : std::false_type {};

	template <typename... Ts>
	struct IsEvent<Event<Ts...>> : std::true_type {};

	template <typename T>
	struct ParamTrait
	{
		using decayed = std::remove_cv_t<std::remove_reference_t<T>>;
		static constexpr bool is_query = IsSparkQuery<decayed>::value;
		static constexpr bool is_event = IsEvent<decayed>::value;
		static constexpr bool is_resource = !is_query && !is_event;
	};

	template <typename T>
	struct QueryComponents;

	template <typename... Ts>
	struct QueryComponents<Query<Ts...>>
	{
		static Query<Ts...> Get(Coordinator& coord)
		{
			return coord.template CreateQuery<Ts...>();
		}
	};

	// Helper to iterate index_sequence
	template <size_t... Is, typename F>
	void ForSequence(std::index_sequence<Is...>, F&& func)
	{
		(func(std::integral_constant<size_t, Is>{}), ...);
	}

	namespace detail
	{
		struct ISystem
		{
			virtual ~ISystem() = default;
			virtual void Execute(Application& app) = 0;
			virtual void Execute(Application& app, std::shared_ptr<IEvent> forced_event) = 0;
			virtual SystemExecutionMode GetExecutionMode() const = 0;
		};

		template <typename T>
		struct System;

		template <typename Arg>
		auto ComputeArgument(Application& app, std::shared_ptr<IEvent> forced_event);
	}

	// For convenience
	template <typename T>
	using Ref = T&;

	template <typename T>
	using CRef = const T&;

	class Application
	{
	public:
		Application(GraphicsApi gapi,
			const std::string& title,
			int32_t win_width,
			int32_t win_height,
			bool win_vsync = false)
			: m_mod_manager(this)
			, m_gapi(gapi)
		{
			m_layer_stack.PushLayer<WindowLayer>(gapi, title, win_width, win_height, win_vsync);
			m_layer_stack.PushLayer<RendererLayer>(gapi, m_command_queue);
			m_layer_stack.PushLayer<EventLayer>(m_event_queue);
			AddResource(*this);
			AddResource(m_coordinator);
			AddResource(m_thread_pool);
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
			DispatchSystemsForPhase(SystemPhase::ON_START);
			m_layer_stack.Start();
			return *this;
		}

		Application& Run()
		{
			while (IsRunning())
			{
				DispatchSystemsForPhase(SystemPhase::BEFORE_UPDATE);
				m_layer_stack.Update(m_dt);
				DispatchSystemsForPhase(SystemPhase::UPDATE);
				DispatchSystemsForPhase(SystemPhase::AFTER_UPDATE);
			}
			return *this;
		}

		Application& Close()
		{
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
			{
				return *this;
			}
			auto mod_instance = m_mod_manager.LoadModInstance(handle);
			if (!mod_instance)
			{
				m_mod_manager.UnloadLibrary(handle);
			}
			return *this;
		}

		template <ValidCommand T, typename... Args>
		Application& SubmitCommand(Args&&... args)
		{
			m_command_queue.template SubmitCommand<T>(std::forward<Args>(args)...);
			return *this;
		}

		template <ValidCommand T>
		Application& SubmitCommand(T&& command)
		{
			m_command_queue.template SubmitCommand<T>(std::forward<T>(command));
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

		template <typename... Funcs>
		Application& RegisterSystems(Funcs&&... funcs)
		{
			static_assert(sizeof...(Funcs) > 0, "RegisterSystems requires at least one argument.");

			constexpr bool last_is_settings =
				std::is_same_v<
				std::decay_t<std::tuple_element_t<sizeof...(Funcs) - 1,
				std::tuple<Funcs...>>>,
				SystemSettings
				>;

			if constexpr (last_is_settings)
			{
				constexpr size_t num_funcs = sizeof...(Funcs) - 1;
				auto args_tuple = std::forward_as_tuple(std::forward<Funcs>(funcs)...);
				const SystemSettings& settings = std::get<num_funcs>(args_tuple);
				RegisterSystemsImpl(args_tuple, std::make_index_sequence<num_funcs>{}, settings);
			}
			else
			{
				RegisterSystems(std::forward<Funcs>(funcs)..., SystemSettings{});
			}
			return *this;
		}

		template <typename Tuple, size_t... Is>
		Application& RegisterSystemsImpl(Tuple&& tuple, std::index_sequence<Is...>, const SystemSettings& settings)
		{
			(RegisterSystem(std::get<Is>(tuple), settings), ...);
			return *this;
		}

		template <typename Func>
		Application& RegisterSystem(Func&& system_func, const SystemSettings& settings = {})
		{
			using F = std::decay_t<Func>;
			using traits = FunctionTraits<F>;
			constexpr size_t arity = traits::arity;

			bool has_event_param = false;
			ForSequence(std::make_index_sequence<arity>{}, [&](auto i)
				{
					using Arg = typename traits::template arg<i>;
					if constexpr (ParamTrait<Arg>::is_event)
					{
						has_event_param = true;
					}
				});

			if (has_event_param)
			{
				auto system = std::make_unique<detail::System<F>>(std::forward<Func>(system_func), settings);
				ForSequence(std::make_index_sequence<arity>{}, [&](auto i)
					{
						using Arg = typename traits::template arg<i>;
						if constexpr (ParamTrait<Arg>::is_event)
						{
							using RealEventT = std::decay_t<Arg>;
							SubscribeAuto<RealEventT>(m_event_queue,
								[this, system_ptr = system.get()](RealEventT& typed_event)
								{
									RealEventT local_copy = typed_event;
									if (system_ptr->GetExecutionMode() == SystemExecutionMode::MULTITHREADED_ASYNC)
									{
										m_thread_pool.Enqueue(system_ptr->m_priority,
											[this, system_ptr, local_copy]() mutable
											{
												auto forced_ev = std::make_shared<RealEventT>(local_copy);
												system_ptr->Execute(*this, forced_ev);
											});
									}
									else if (system_ptr->GetExecutionMode() == SystemExecutionMode::MULTITHREADED_SYNC)
									{
										auto task = m_thread_pool.Enqueue(system_ptr->m_priority,
											[this, system_ptr, local_copy]() mutable
											{
												auto forced_ev = std::make_shared<RealEventT>(local_copy);
												system_ptr->Execute(*this, forced_ev);
											});
										task.result.get();
									}
									else
									{
										auto forced_ev = std::make_shared<RealEventT>(local_copy);
										system_ptr->Execute(*this, forced_ev);
									}
								});
						}
					});
				m_event_systems.push_back(std::move(system));
			}
			else
			{
				auto system = std::make_unique<detail::System<F>>(std::forward<Func>(system_func), settings);
				m_systems[settings.phase].push_back(std::move(system));
			}
			return *this;
		}

		template <typename T>
		bool HasResource() const
		{
			using pointer_type = std::remove_reference_t<T>*;
			return (m_resources.find(std::type_index(typeid(pointer_type))) != m_resources.end());
		}

		template <typename T>
		threading::AutoLockable<T*>& GetResource()
		{
			using pointer_type = std::remove_reference_t<T>*;

			auto it = m_resources.find(std::type_index(typeid(pointer_type)));
			if (it == m_resources.end())
			{
				throw std::runtime_error("Requested resource not found!");
			}

			auto ptr_any = std::any_cast<std::shared_ptr<threading::AutoLockable<pointer_type>>>(it->second);
			if (!ptr_any)
			{
				throw std::runtime_error("any_cast to std::shared_ptr<AutoLockable<T*>> failed!");
			}

			// Return a reference so you can do .Lock() or .CLock()
			return *ptr_any;
		}


		template <typename T>
		Application& AddResource(T& resource)
		{
			// We always store ‘T*’ in the map, not a copy of T.
			// So ‘Application&’ becomes ‘Application*’, ‘Coordinator&’ → ‘Coordinator*’, etc.
			using pointer_type = std::remove_reference_t<T>*;

			m_resources[std::type_index(typeid(pointer_type))] =
				std::make_shared<threading::AutoLockable<pointer_type>>(std::addressof(resource));
			return *this;
		}



		Application& SetDeltaTime(DeltaTime<double> dt)
		{
			m_dt = dt;
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
			if (it == m_systems.end()) return;

			std::vector<std::future<void>> sync_tasks;
			for (auto& system_ptr : it->second)
			{
				switch (system_ptr->GetExecutionMode())
				{
				case SystemExecutionMode::SINGLE_THREADED:
					system_ptr->Execute(*this);
					break;
				case SystemExecutionMode::MULTITHREADED_SYNC:
				{
					auto task = m_thread_pool.Enqueue(SystemPriority::NORMAL,
						[this, &system_ptr]()
						{
							system_ptr->Execute(*this);
						});
					sync_tasks.emplace_back(std::move(task.result));
					break;
				}
				case SystemExecutionMode::MULTITHREADED_ASYNC:
				{
					m_thread_pool.Enqueue(SystemPriority::NORMAL,
						[this, &system_ptr]()
						{
							system_ptr->Execute(*this);
						});
					break;
				}
				}
			}
			for (auto& fut : sync_tasks)
			{
				fut.get();
			}
		}

		IRenderer& GetRenderer()
		{
			return GetRendererLayer().GetRenderer();
		}

		Application& ChangeGraphicsApi(GraphicsApi gapi)
		{
			if (gapi == m_gapi) return *this;
			GetWindowLayer().SetGraphicsApi(gapi);
			GetRendererLayer().SetGraphicsApi(gapi);
			return *this;
		}

	private:
		template <typename E>
		struct EventExtractor;

		template <typename... Ts>
		struct EventExtractor<Event<Ts...>>
		{
			using pack = std::tuple<Ts...>;
		};

		template <typename E, typename F>
		void SubscribeAuto(EventQueue& eq, F&& user_callback)
		{
			using pack_t = typename EventExtractor<E>::pack;
			SubscribeAutoImpl<E>(
				eq,
				std::forward<F>(user_callback),
				std::make_index_sequence<std::tuple_size_v<pack_t>>{}
			);
		}

		template <typename E, typename F, size_t... Is>
		void SubscribeAutoImpl(EventQueue& eq, F&& user_callback, std::index_sequence<Is...>)
		{
			using pack_t = typename EventExtractor<E>::pack;
			eq.template Subscribe<std::tuple_element_t<Is, pack_t>...>(
				[cb = std::forward<F>(user_callback)](E& ev) mutable
				{
					cb(ev);
				}
			);
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
		LayerStack m_layer_stack;
		CommandQueue m_command_queue;
		EventQueue m_event_queue;
		ModManager m_mod_manager;
		threading::ThreadPool m_thread_pool;
		GraphicsApi m_gapi;
		DeltaTime<double> m_dt;
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
		template <typename Func>
		struct System : ISystem
		{
			Func m_func;
			SystemExecutionMode m_execution_mode;
			SystemPriority m_priority;

			System(Func&& f, const SystemSettings& settings)
				: m_func(std::forward<Func>(f))
				, m_execution_mode(settings.execution_mode)
				, m_priority(settings.priority)
			{
			}

			void Execute(Application& app) override
			{
				ExecuteImpl(app, nullptr);
			}

			void Execute(Application& app, std::shared_ptr<IEvent> forced_event) override
			{
				ExecuteImpl(app, forced_event);
			}

			SystemExecutionMode GetExecutionMode() const override
			{
				return m_execution_mode;
			}

		private:
			// create a tuple of ArgHolders so that resource locks
			// are held for the duration of the system function call.
			void CallFunc(Application& app, std::shared_ptr<IEvent> forced_evt)
			{
				using traits = FunctionTraits<Func>;
				constexpr size_t arity = traits::arity;
				auto holders = [&]<std::size_t... Is>(std::index_sequence<Is...>)
				{
					return std::make_tuple(MakeArgHolder<typename traits::template arg<Is>>(app, forced_evt)...);
				}(std::make_index_sequence<arity>{});
				std::apply([this](auto& ... holder)
					{
						m_func(holder.Get()...);
					}, holders);
			}

			void ExecuteImpl(Application& app, std::shared_ptr<IEvent> forced_evt)
			{
				CallFunc(app, forced_evt);
			}
		};

		template <typename Arg>
		Arg ComputeArgumentImpl(Application& app, std::shared_ptr<IEvent> forced_evt, std::true_type /* is_event */)
		{
			if (!forced_evt)
			{
				throw std::runtime_error("System expects an event, but no forced event was provided!");
			}
			using ArgValue = std::remove_reference_t<Arg>;
			auto typed_event = std::dynamic_pointer_cast<ArgValue>(forced_evt);
			if (!typed_event)
			{
				throw std::runtime_error("Could not cast forced event to the expected multi-event type!");
			}
			return *typed_event;
		}

		template <typename Arg>
		auto ComputeArgumentImpl(Application& app, std::shared_ptr<IEvent> /* forced_evt */, std::false_type /* is_event */)
		{
			if constexpr (ParamTrait<Arg>::is_query)
			{
				return QueryComponents<Arg>::Get(app.GetCoordinator());
			}
			else if constexpr (ParamTrait<Arg>::is_resource)
			{
				using bare_t = std::remove_reference_t<Arg>;
				if (app.HasResource<bare_t>())
				{
					auto& auto_lockable = app.GetResource<bare_t>();
					if constexpr (std::is_const_v<bare_t>)
					{
						return auto_lockable.CLock();
					}
					else
					{
						return auto_lockable.Lock();
					}
				}
				else if (app.HasResource<bare_t*>())
				{
					auto& auto_lockable = app.GetResource<bare_t*>();
					if constexpr (std::is_const_v<bare_t>)
					{
						return auto_lockable.CLock();
					}
					else
					{
						return auto_lockable.Lock();
					}
				}
				else
				{
					throw std::runtime_error("Dependency must be registered as a resource for automatic locking!");
				}
			}
			else if constexpr (ParamTrait<Arg>::is_coordinator)
			{
				return app.GetCoordinator();
			}
			else if constexpr (ParamTrait<Arg>::is_thread_pool)
			{
				return app.GetThreadPool();
			}
			else if constexpr (ParamTrait<Arg>::is_application)
			{
				return app;
			}
			else
			{
				static_assert(sizeof(Arg) == 0, "Unhandled Arg type in ComputeArgument!");
			}
		}

		template <typename Arg>
		auto ComputeArgument(Application& app, std::shared_ptr<IEvent> forced_evt)
		{
			constexpr bool is_event = ParamTrait<Arg>::is_event;
			return ComputeArgumentImpl<Arg>(app, forced_evt, std::integral_constant<bool, is_event>{});
		}


		template <typename Arg>
		auto MakeArgHolder(Application& app, std::shared_ptr<IEvent> forced_evt)
		{
			if constexpr (ParamTrait<Arg>::is_resource)
			{
				using ResourceType = std::remove_reference_t<Arg>;
				if constexpr (std::is_const_v<ResourceType>)
				{
					auto lock = app.GetResource<ResourceType>().CLock();
					return ArgHolder<Arg, true>(std::move(lock));
				}
				else
				{
					auto lock = app.GetResource<ResourceType>().Lock();
					return ArgHolder<Arg, true>(std::move(lock));
				}
			}
			else
			{
				auto value = ComputeArgument<Arg>(app, forced_evt);
				return ArgHolder<Arg, false>(std::move(value));
			}
		}
	} // namespace detail

} // namespace spark

#endif // SPARK_APPLICATION_HPP
