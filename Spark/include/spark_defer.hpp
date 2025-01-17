#ifndef SPARK_DEFER_HPP
#define SPARK_DEFER_HPP
#include "spark_pch.hpp"

namespace spark
{
    template <typename... Args>
    struct DeferredState
    {
        static inline bool initialized = false;

        template <typename T>
        using DecayOrWrap = std::conditional_t<std::is_reference_v<T>, std::reference_wrapper<std::remove_reference_t<T>>, typename std::decay<T>::type>;

        static inline std::optional<std::tuple<DecayOrWrap<Args>...>> values;

        template <typename... T>
        static void Initialize(T&&... args)
        {
            // Forward arguments and wrap references as needed
            values.emplace(std::make_tuple(DecayOrWrap<T>(std::forward<T>(args))...));
            initialized = true;
        }

        static void Execute(auto&& func)
        {
            if (initialized && values.has_value())
            {
                // Use std::apply to invoke the function with tuple values
                std::apply(std::forward<decltype(func)>(func), values.value());
                values.reset();
                initialized = false;
            }
        }

        static void Reset()
        {
            values.reset();
            initialized = false;
        }
    };
}

#endif
