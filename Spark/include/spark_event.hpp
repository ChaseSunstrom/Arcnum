#ifndef SPARK_EVENT_HPP
#define SPARK_EVENT_HPP

#include "spark_pch.hpp"

// Base interface
namespace spark
{
    struct IEvent
    {
        virtual ~IEvent() = default;

        // Called to identify which type is active, plus a pointer
        virtual void VisitActive(std::function<void(std::type_index, void*)> callback) = 0;
    };

    // Forward declare
    template <typename... Ts>
    class Event;

    //-----------------------------------------------------
    // Single-type specialization: Event<T>
    //-----------------------------------------------------
    template <typename T>
    class Event<T> : public IEvent
    {
    public:
        using variant_type = std::variant<std::shared_ptr<T>>;

        Event() = default;

        // Construct from a T by value
        explicit Event(const T& value)
            : m_variant(std::make_shared<T>(value))
        {
        }

        // Construct from a shared_ptr<T>
        explicit Event(std::shared_ptr<T> ptr)
            : m_variant(ptr)
        {
        }

        const variant_type& GetVariant() const { return m_variant; }
        variant_type& GetVariant() { return m_variant; }

        // IEvent override
        void VisitActive(std::function<void(std::type_index, void*)> callback) override
        {
            auto& sp = std::get<std::shared_ptr<T>>(m_variant);
            if (sp)
            {
                callback(std::type_index(typeid(*sp)), sp.get());
            }
            else
            {
                callback(std::type_index(typeid(T)), nullptr);
            }
        }

        // For convenience, if you want a direct check or get:
        bool Holds() const { return (std::get_if<std::shared_ptr<T>>(&m_variant) != nullptr); }

    private:
        variant_type m_variant;
    };

    // ------------------------------------------------------------
    // Helper to see if T is among [Us...]
    // ------------------------------------------------------------
    template <typename T, typename... Us>
    struct IsAnyOf : std::bool_constant<(std::is_same_v<T, Us> || ...)> {};

    // For smaller => bigger multi check
    // We say "All of Others... appear in T, U, More..."
    // So if Others... = (Blah), and T,U,More... = (Blah, Accel), we pass.
    template <typename... As>
    struct AllAreIn;

    template <typename A, typename... As>
    struct AllAreIn<A, As...>
    {
        template <typename... Bs>
        static constexpr bool value = IsAnyOf<A, Bs...>::value&& AllAreIn<As...>::template value<Bs...>;
    };

    template <>
    struct AllAreIn<>
    {
        template <typename... Bs>
        static constexpr bool value = true;
    };

    template <typename... As, typename... Bs>
    concept AllIn = AllAreIn<As...>::template value<Bs...>;

    //-----------------------------------------------------
    // Multi-type specialization: Event<T, U, More...>
    //-----------------------------------------------------
    template <typename T, typename U, typename... More>
    class Event<T, U, More...> : public IEvent
    {
    public:
        using variant_type = std::variant<std::shared_ptr<T>, std::shared_ptr<U>, std::shared_ptr<More>...>;

        Event() = default;

        // Construct from a shared_ptr<X> if X is in [T, U, More...]
        template <typename X>
            requires IsAnyOf<X, T, U, More...>::value
        explicit Event(std::shared_ptr<X> ptr)
            : m_variant(ptr)
        {
        }

        // Return the underlying variant
        const variant_type& GetVariant() const { return m_variant; }
        variant_type& GetVariant() { return m_variant; }

        // IEvent override
        void VisitActive(std::function<void(std::type_index, void*)> callback) override
        {
            std::visit(
                [&](auto& sp)
                {
                    if (sp)
                    {
                        callback(std::type_index(typeid(*sp)), sp.get());
                    }
                    else
                    {
                        // null pointer for that type
                        using sp_type = std::remove_reference_t<decltype(sp)>;
                        callback(std::type_index(typeid(std::remove_pointer_t<decltype(sp.get())>)), nullptr);
                    }
                },
                m_variant
            );
        }

        //-------------------------------------------------
        // Smaller multi => bigger multi
        // e.g. Event<Blah> => Event<Blah, Accel>
        // or   Event<Blah, Foo> => Event<Blah, Foo, Accel>
        //-------------------------------------------------
        template <typename... Others>
            requires AllIn<Others..., T, U, More...>
        Event(const Event<Others...>& other)
        {
            // whichever pointer is active in "other", store that pointer in this->m_variant
            std::visit(
                [this](auto&& active_ptr)
                {
                    this->m_variant = active_ptr;
                },
                other.GetVariant()
            );
        }

        //-------------------------------------------------
        // Single => bigger multi
        // e.g. Event<Acceleration> => Event<Blah,Acceleration>
        //-------------------------------------------------
        template <typename Single>
            requires IsAnyOf<Single, T, U, More...>::value
        Event(const Event<Single>& single_event)
        {
            // single_event has a variant with exactly one type: shared_ptr<Single>
            const auto& var_single = single_event.GetVariant();
            // e.g. var_single is a variant<std::shared_ptr<Single>>
            // We'll get that pointer
            const auto& sp = std::get<std::shared_ptr<Single>>(var_single);
            // store it in our multi-variant
            m_variant = sp;
        }

        // For convenience
        template <typename X>
            requires IsAnyOf<X, T, U, More...>::value
        bool Holds() const
        {
            return std::holds_alternative<std::shared_ptr<X>>(m_variant);
        }

        template <typename X>
            requires IsAnyOf<X, T, U, More...>::value
        X& Get()
        {
            auto& sp = std::get<std::shared_ptr<X>>(m_variant);
            if (!sp) { throw std::runtime_error("Null pointer for that type in multi-event."); }
            return *sp;
        }

    private:
        variant_type m_variant;
    };

} // namespace spark

#endif // SPARK_EVENT_HPP
