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
	enum class SystemStage { Start, Stop, First, PreUpdate, Update, PostUpdate, PreRender, Render, PostRender, Last, Count };

	class Application;

	// Base system interface
	class ISystem {
	  public:
		virtual ~ISystem()                     = default;
		virtual void Execute(Application& app) = 0;
		virtual void Start() {}
		virtual void Shutdown() {}
	};

	// Parameter extraction traits
	template<typename T, typename = void> struct SystemParamTrait {
		using Type = T;
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

	// Event specializations
	template<typename... Events> struct SystemParamTrait<const MultiEventPtr<Events...>&> {
		using Type = const MultiEventPtr<Events...>&;
		static Type Extract(Application&) { return nullptr; }
	};

	template<typename T> struct SystemParamTrait<const EventPtr<T>&> {
		using Type = const EventPtr<T>&;
		static Type Extract(Application&) { return nullptr; }
	};

	// Parameter extraction utility
	template<typename Func> class SystemParameters {
	  private:
		template<typename F, typename Tuple, std::size_t... Is> static decltype(auto) apply_impl(F& func, Tuple&& tuple, std::index_sequence<Is...>) {
			return func(std::get<Is>(std::forward<Tuple>(tuple))...);
		}

		template<typename Tuple, std::size_t... Is> static auto extract_impl(Application& app, std::index_sequence<Is...>) {
			if constexpr (sizeof...(Is) == 0) {
				return std::tuple<>();
			} else {
				return std::tuple<typename SystemParamTrait<std::tuple_element_t<Is, Tuple>>::Type...>(SystemParamTrait<std::tuple_element_t<Is, Tuple>>::Extract(app)...);
			}
		}

	  public:
		static auto Extract(Application& app) {
			using Traits = FunctionTraits<std::decay_t<Func>>;
			return extract_impl<typename Traits::ArgsTuple>(app, std::make_index_sequence<Traits::Arity>{});
		}

		template<typename F, typename Tuple> static decltype(auto) Apply(F& func, Tuple&& tuple) {
			using Traits = FunctionTraits<std::decay_t<F>>;
			return apply_impl(func, std::forward<Tuple>(tuple), std::make_index_sequence<Traits::Arity>{});
		}
	};

	// Universal System implementation
	template<typename Func> class System : public ISystem {
	  public:
		explicit System(Func func, SystemStage stage = SystemStage::Update)
			: m_func(std::move(func))
			, m_stage(stage) {}

		void Execute(Application& app) override {
			auto params = SystemParameters<Func>::Extract(app);
			SystemParameters<Func>::Apply(m_func, params);
		}

		SystemStage GetStage() const { return m_stage; }

	  private:
		Func        m_func;
		SystemStage m_stage;
	};
	// Factory function for system creation
	template<typename Func> auto MakeSystem(Func&& func) { return MakeUnique<System<std::decay_t<Func>>>(std::forward<Func>(func)); }

	// Event System specialization
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
		template<typename Event> void RegisterSingleHandler() {
			m_event_handler->SubscribeToEvent<Event>([this](const EventPtr<Event>& event) {
				auto multi_event = MakeShared<MultiEvent<Event>>(event);
				m_func(m_app, multi_event);
			});
		}

		void RegisterEventHandlers() {
			using FuncTraits  = FunctionTraits<std::decay_t<Func>>;
			using EventPtrArg = std::tuple_element_t<1, typename FuncTraits::ArgsTuple>;
			using EventTypes  = typename std::remove_cvref_t<EventPtrArg>::element_type;

			std::apply([this](auto... types) { (RegisterSingleHandler<typename decltype(types)::type>(), ...); }, EventTypes::GetEventTypes());
		}

		Func                 m_func;
		RefPtr<EventHandler> m_event_handler;
		Application&         m_app;
	};

	// Factory function for event system creation
	template<typename Func> auto MakeEventSystem(Application& app, Func&& func, RefPtr<EventHandler> event_handler) {
		return MakeUnique<EventSystem<std::decay_t<Func>>>(app, std::forward<Func>(func), event_handler);
	}

} // namespace Spark

#endif