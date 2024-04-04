#ifndef SPARK_RESULT_HPP
#define SPARK_RESULT_HPP

#include "../spark.hpp"
namespace spark
{
	template <typename E>
	concept exception = std::is_base_of_v<std::runtime_error, E>;

	template <typename T>
	class ok
	{
	public:
		explicit ok(const T& value) : m_value(value) {}
		explicit ok(T&& value) : m_value(std::move(value)) {} 
		T& get_value() 
		{
			return m_value;
		}
		const T& get_value() const 
		{
			return m_value;
		}
	private:
		T m_value;
	};

	template <exception E>
	class error
	{
	public:
		explicit error(const E& error) : m_error(error) {}
		explicit error(E&& error) : m_error(std::move(error)) {}
		E& get_error() 
		{
			return m_error;
		}
		const E& get_error() const 
		{
			return m_error;
		} 
	private:
		E m_error;
	};

	template <typename T, exception E>
	class result
	{
	public:
		template <typename U>
		result(ok<U>&& value) : m_value(std::forward<ok<U>>(value)) {}

		template <typename F>
		result(error<F>&& error) : m_value(std::forward<error<F>>(error)) {}

		bool is_ok() const 
		{
			return std::holds_alternative<ok<T>>(m_value);
		}
		bool is_error() const 
		{
			return std::holds_alternative<error<E>>(m_value);
		}

		T& get_value() 
		{
			return std::get<ok<T>>(m_value).get_value();
		}
		const T& get_value() const 
		{
			return std::get<ok<T>>(m_value).get_value();
		}

		E& get_error() 
		{
			return std::get<error<E>>(m_value).get_error();
		}
		const E& get_error() const 
		{
			return std::get<error<E>>(m_value).get_error();
		}

		T& value_or(T& default_value) 
		{
			if (is_ok()) 
			{
				return get_value();
			}
			return default_value;
		}

		template <typename Func>
		auto map(Func&& func) -> result<decltype(func(std::declval<T>())), E> 
		{
			if (is_ok()) 
			{
				return ok<decltype(func(std::declval<T>()))>(func(get_value()));
			}
			return error<E>(get_error());
		}

		T value_or(T&& default_value) const 
		{
			if (is_ok()) 
			{
				return get_value();
			}
			return std::forward<T>(default_value);
		}

		template <typename Func>
		auto and_then(Func&& func) -> decltype(func(std::declval<T>())) 
		{
			if (is_ok()) 
			{
				return func(get_value());
			}
			return *this;
		}

		template <typename Func>
		auto or_else(Func&& func) -> decltype(func(std::declval<E>())) 
		{
			if (is_error()) 
			{
				return func(get_error());
			}
			return *this;
		}

		template <typename OkFunc, typename ErrorFunc>
		auto match(OkFunc&& ok_func, ErrorFunc&& error_func) -> decltype(ok_func(std::declval<T>())) 
		{
			if (is_ok()) 
			{
				return ok_func(get_value());
			}
			else 
			{
				return error_func(get_error());
			}
		}
	private:
		std::variant<ok<T>, error<E>> m_value;
	};
}

#endif