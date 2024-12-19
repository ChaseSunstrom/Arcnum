#ifndef SPARK_SYSTEM_HPP
#define SPARK_SYSTEM_HPP
#include <core/event/event.hpp>
#include <core/event/event_handler.hpp>
#include <core/pch.hpp>
#include <core/resource/asset.hpp>
#include <core/util/classic/function_traits.hpp>
#include <functional>
#include <tuple>
#include <type_traits>

namespace Spark {
	class Application;

	// Type traits and helper concepts
	template<typename T>
	concept IsApplicationRef = std::is_same_v<std::remove_cv_t<T>, Application&>;

	class ISystem {
	  public:
		virtual ~ISystem()                     = default;
		virtual void Execute(Application& app) = 0;
		virtual void Start() {}
		virtual void Shutdown() {}
	};

	// Advanced parameter extraction traits
	template<typename T, typename = void> struct SystemParamTrait {
		using Type = T;
		// Default extraction for unknown types with a constructor taking Application
		static Type Extract(Application& app) {
			if constexpr (std::is_constructible_v<T, Application&>) {
				return T(app);
			} else {
				return T{};
			}
		}
	};

	// Specialization for Application reference
	template<> struct SystemParamTrait<Application&> {
		using Type = Application&;
		static Type Extract(Application& app) { return app; }
	};

	// Parameter extraction utility
	template<typename Func> class SystemParameters {
	  private:
		// Specialized invoke helper
		template<typename F, typename... Args> static void invoke_impl(F&& func, Args&&... args) {
			if constexpr (std::is_invocable_v<F, Args...>) {
				std::invoke(std::forward<F>(func), std::forward<Args>(args)...);
			} else {
				func(std::forward<Args>(args)...);
			} 
		}

		// Type-erased parameter extraction
		template<typename F, std::size_t... Is> static auto extract_impl(Application& app, F&& func, std::index_sequence<Is...>) {
			using FuncTraits = FunctionTraits<std::decay_t<F>>;
			using ParamTuple = typename FuncTraits::TupleType;

			return std::tuple<typename SystemParamTrait<std::tuple_element_t<Is, ParamTuple>>::Type... > (SystemParamTrait<std::tuple_element_t<Is, ParamTuple>>::Extract(app)...);
		}

	  public:
		template<typename F> static auto Extract(Application& app, F&& func) {
			using FuncTraits           = FunctionTraits<std::decay_t<F>>;
			constexpr auto param_count = FuncTraits::Arity;
			return extract_impl(app, std::forward<F>(func), std::make_index_sequence<param_count>{});
		}

		template<typename F, typename... Args> static void Invoke(F&& func, Args&&... args) { invoke_impl(std::forward<F>(func), std::forward<Args>(args)...); }
	};

	// Universal System implementation
	template<typename Func> class System : public ISystem {
	  public:
		explicit System(Func func)
			: m_func(std::move(func)) {}

		void Execute(Application& app) override {
			auto params = SystemParameters<Func>::Extract(app, m_func);
			std::apply([this, &app](auto&&... extracted_params) { SystemParameters<Func>::Invoke(m_func, app, std::forward<decltype(extracted_params)>(extracted_params)...); }, params);
		}

	  private:
		Func m_func;
	};

	// Event System implementation
	template<typename Func> class EventSystem : public ISystem {
	  public:
		EventSystem(Application& app, Func func, RefPtr<EventHandler> event_handler)
			: m_func(std::move(func))
			, m_event_handler(event_handler)
			, m_app(app) {
			RegisterEventHandlers();
		}

		void Execute(Application&) override {} // Events are handled by registration

	  private:
		void RegisterEventHandlers() {
			// Detect event parameter and register appropriately
			auto register_event = [this](auto event_type) {
				using EventType = decltype(event_type);
				if constexpr (IsEventPtr<EventType>) {
					using BaseEventType = typename EventType::EventType;
					m_event_handler->SubscribeToEvent<BaseEventType>([this](const EventPtr<BaseEventType>& event) {
						auto params = SystemParameters<Func>::Extract(m_app, [this, &event](auto&&... args) { return m_func(m_app, event, std::forward<decltype(args)>(args)...); });
						std::apply([this, &event](auto&&... extracted_params) { SystemParameters<Func>::Invoke(m_func, m_app, event, std::forward<decltype(extracted_params)>(extracted_params)...); }, params);
					});
				}
			};

			// Reflection of first parameter for event detection
			using FuncTraits = FunctionTraits<Func>;
			using FirstParam = std::tuple_element_t<0, typename FuncTraits::TupleType>;
			register_event(FirstParam{});
		}

		Func                 m_func;
		RefPtr<EventHandler> m_event_handler;
		Application&         m_app;
	};

	// Factory functions for system creation
	template<typename Func> auto MakeSystem(Func&& func) { return MakeUnique<System<std::decay_t<Func>>>(std::forward<Func>(func)); }

	template<typename Func> auto MakeEventSystem(Application& app, Func&& func, RefPtr<EventHandler> event_handler) {
		return MakeUnique<EventSystem<std::decay_t<Func>>>(app, std::forward<Func>(func), event_handler);
	}
} // namespace Spark

#endif