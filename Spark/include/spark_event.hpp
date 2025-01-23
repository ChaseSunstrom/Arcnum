#ifndef SPARK_EVENT_HPP
#define SPARK_EVENT_HPP

#include "spark_pch.hpp"

namespace spark
{
    struct IEvent
    {
        virtual ~IEvent() = default;

        // This method should call `callback(type_index, void*)` 
        // indicating which data type is active, plus the pointer to that data.
        virtual void VisitActive(std::function<void(std::type_index, void*)> callback) = 0;
    };

    template <typename... Ts>
    class Event;

    //-------------------------------------------------------------
    // SINGLE-TYPE SPECIALIZATION: Event<T>
    //-------------------------------------------------------------
    template <typename T>
    class Event<T> : public IEvent
    {
    public:
        using Variant = std::variant<std::shared_ptr<T>>;

        // Default constructor
        Event() = default;

        // Construct from a shared_ptr<T>
        explicit Event(std::shared_ptr<T> ptr)
            : m_variant(ptr)
        {
        }

        explicit Event(const T& value)
            : m_variant(std::make_shared<T>(value))
        {
        }

        // Access the variant
        const Variant& GetVariant() const { return m_variant; }
        Variant& GetVariant() { return m_variant; }

        // operator-> for convenience
        T* operator->()
        {
            return std::get<std::shared_ptr<T>>(m_variant).get();
        }
        const T* operator->() const
        {
            return std::get<std::shared_ptr<T>>(m_variant).get();
        }

        // operator* => T&
        T& operator*()
        {
            return *std::get<std::shared_ptr<T>>(m_variant);
        }
        const T& operator*() const
        {
            return *std::get<std::shared_ptr<T>>(m_variant);
        }

        // IEvent method: identify that we only hold T
        virtual void VisitActive(std::function<void(std::type_index, void*)> callback) override
        {
            auto& sp = std::get<std::shared_ptr<T>>(m_variant);
            if (sp)
            {
                // typeid(*sp) => typeid(T) if sp not empty
                callback(std::type_index(typeid(*sp)), sp.get());
            }
            else
            {
                // If null, still pass typeid(T), but pointer is nullptr
                callback(std::type_index(typeid(T)), nullptr);
            }
        }

        // Conversion from multi => single if you wanted
        // but typically you'd do single => multi. We'll skip or block it.
        template <typename... Others>
        Event(const Event<Others...>&)
        {
            static_assert(sizeof...(Others) == -1,
                "Multi => single not supported. Use single => multi instead.");
        }

    private:
        Variant m_variant;
};
    
    template <typename T, typename... Us>
    struct IsAnyOf : std::bool_constant<(std::is_same_v<T, Us> || ...)> { };

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

    // Now define the multi-type partial specialization
    template <typename T, typename U, typename... More>
    class Event<T, U, More...> : public IEvent
    {
    public:
        using Variant = std::variant<std::shared_ptr<T>, std::shared_ptr<U>, std::shared_ptr<More>...>;

        Event() = default;

        template <typename X>
            requires IsAnyOf<X, T, U, More...>::value
        explicit Event(std::shared_ptr<X> ptr)
            : m_variant(ptr)
        {
        }

        const Variant& GetVariant() const { return m_variant; }
        Variant& GetVariant() { return m_variant; }

        // Checks if we hold a certain type X
        template <typename X>
            requires IsAnyOf<X, T, U, More...>::value
        bool Holds() const
        {
            return std::holds_alternative<std::shared_ptr<X>>(m_variant);
        }

        // Return reference to X
        template <typename X>
            requires IsAnyOf<X, T, U, More...>::value
        X& Get()
        {
            auto& sp = std::get<std::shared_ptr<X>>(m_variant);
            if (!sp) { throw std::runtime_error("Null pointer stored for this type."); }
            return *sp;
        }
        template <typename X>
            requires IsAnyOf<X, T, U, More...>::value
        const X& Get() const
        {
            auto& sp = std::get<std::shared_ptr<X>>(m_variant);
            if (!sp) { throw std::runtime_error("Null pointer stored for this type."); }
            return *sp;
        }

        // TryGet => pointer or nullptr
        template <typename X>
            requires IsAnyOf<X, T, U, More...>::value
        X* TryGet()
        {
            if (!std::holds_alternative<std::shared_ptr<X>>(m_variant))
                return nullptr;
            auto& sp = std::get<std::shared_ptr<X>>(m_variant);
            return sp.get();
        }
        template <typename X>
            requires IsAnyOf<X, T, U, More...>::value
        const X* TryGet() const
        {
            if (!std::holds_alternative<std::shared_ptr<X>>(m_variant))
                return nullptr;
            auto& sp = std::get<std::shared_ptr<X>>(m_variant);
            return sp.get();
        }

        // IEvent: figure out which pointer is active, call callback
    	void VisitActive(std::function<void(std::type_index, void*)> callback) override
        {
            std::visit(
                [&](auto& sp)
                {
                    using actual_t = std::decay_t<decltype(sp)>; // e.g. std::shared_ptr<X>
                    if (sp)
                    {
                        callback(std::type_index(typeid(*sp)), sp.get());
                    }
                    else
                    {
                        // null pointer, pass typeid(X) and nullptr
                        // (The line below uses 'std::remove_pointer_t' to ensure it’s the correct type)
                        callback(std::type_index(typeid(std::remove_pointer_t<decltype(sp.get())>)), nullptr);
                    }
                },
                m_variant
            );
        }

        // Conversion from smaller => bigger
        template <typename... Others>
            requires AllIn<Others..., T, U, More...>
        Event(const Event<Others...>& other)
        {
            std::visit(
                [this](auto&& arg_ptr)
                {
                    this->m_variant = arg_ptr;
                },
                other.GetVariant()
            );
        }

        // Conversion from single => multi
        template <typename Single>
            requires IsAnyOf<Single, T, U, More...>::value
        Event(const Event<Single>& single_event)
        {
            const auto& var_single = single_event.GetVariant();
            const auto& sp = std::get<std::shared_ptr<Single>>(var_single);
            m_variant = sp;
        }

    private:
        Variant m_variant;
    };
}

#endif // SPARK_EVENT_HPP
