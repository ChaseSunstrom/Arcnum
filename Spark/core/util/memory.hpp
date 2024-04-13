#ifndef SPARK_MEMORY_HPP
#define SPARK_MEMORY_HPP

#include "../spark.hpp"
#include "../logging/log.hpp"

namespace std
{
	template<typename TO, typename FROM>
	unique_ptr<TO> static_pointer_cast(unique_ptr<FROM>&& old)
	{
		return unique_ptr<TO>((TO*)old.release());
	}
}

// Here because technically this is a smart pointer
namespace spark
{
	template<typename T>
	class unique_shield_ptr;

	template<typename T>
	class unique_proxy
	{
	public:
		T* operator->() { return m_ptr; }
		T& operator*() { return *m_ptr; }

		unique_proxy(const unique_proxy&) = delete;
		unique_proxy& operator=(const unique_proxy&) = delete;

		~unique_proxy() = default;

	private:
		friend class unique_shield_ptr<T>;

		unique_proxy(T* ptr, std::recursive_mutex& mtx) : m_ptr(ptr), m_lock(mtx) {}

		T* m_ptr;
		std::unique_lock<std::recursive_mutex> m_lock;
	};

	template<typename T>
	class unique_shield_ptr
	{
	public:
		explicit unique_shield_ptr(T* ptr = nullptr) : m_data(ptr), m_mutex(new std::recursive_mutex) {}

		~unique_shield_ptr()
		{
			delete m_data;
			delete m_mutex;
		}

		// Delete copy semantics
		unique_shield_ptr(const unique_shield_ptr&) = delete;
		unique_shield_ptr& operator=(const unique_shield_ptr&) = delete;

		unique_shield_ptr(unique_shield_ptr&& other) noexcept
			: m_data(other.m_data), m_mutex(other.m_mutex)
		{
			other.m_data = nullptr;
			other.m_mutex = nullptr;
		}

		unique_shield_ptr& operator=(unique_shield_ptr&& other) noexcept
		{
			if (this != &other)
			{
				std::lock_guard<std::recursive_mutex> lock(*m_mutex); // Lock the current mutex
				delete m_data; // Safely delete the managed object

				std::lock_guard<std::recursive_mutex> other_lock(*other.m_mutex); // Lock the other mutex
				m_data = other.m_data;
				m_mutex = other.m_mutex;
				other.m_data = nullptr;
				other.m_mutex = nullptr;
			}
			return *this;
		}

		T* get() const { return m_data; }

		void reset(T* ptr = nullptr)
		{
			std::unique_lock<std::recursive_mutex> lock(*m_mutex);
			delete m_data;
			m_data = ptr;
		}

		unique_proxy<T> access()
		{
			if (m_mutex)
			{
				return unique_proxy<T>(m_data, *m_mutex);
			}
			else
			{
				SPARK_FATAL("unique_shield_ptr does not have a mutex");
			}
		}

	private:
		T* m_data;
		std::recursive_mutex* m_mutex;
	};

	template<typename T>
	class shared_shield_ptr;

	template<typename T>
	class shared_proxy
	{
	public:
		T* operator->() { return m_ptr.get(); }
		T& operator*() { return *m_ptr.get(); }

		shared_proxy(const shared_proxy&) = delete;
		shared_proxy& operator=(const shared_proxy&) = delete;

		~shared_proxy() = default;

	private:
		friend class shared_shield_ptr<T>;

		shared_proxy(std::shared_ptr<T> ptr, std::recursive_mutex& mtx)
			: m_ptr(std::move(ptr)), m_lock(mtx) {}

		std::shared_ptr<T> m_ptr;
		std::unique_lock<std::recursive_mutex> m_lock;
	};

	template<typename T>
	class shared_shield_ptr
	{
	public:
		explicit shared_shield_ptr(T* ptr = nullptr)
			: m_data(std::shared_ptr<T>(ptr)), m_mutex(std::make_shared<std::recursive_mutex>()) {}

		explicit shared_shield_ptr(std::shared_ptr<T> data, std::shared_ptr<std::recursive_mutex> mutex)
			: m_data(std::move(data)), m_mutex(std::move(mutex)) {}

		shared_shield_ptr(const shared_shield_ptr& other) = default;
		shared_shield_ptr& operator=(const shared_shield_ptr& other) = default;
		shared_shield_ptr(shared_shield_ptr&& other) = default;
		shared_shield_ptr& operator=(shared_shield_ptr&& other) = default;

		T* get() const { return m_data.get(); }
		std::shared_ptr<T> get_shared_ptr() const { return m_data; }
		std::shared_ptr<std::recursive_mutex> get_mutex() const { return m_mutex; }

		void reset(T* ptr = nullptr)
		{
			std::unique_lock<std::recursive_mutex> lock(*m_mutex);
			m_data.reset(ptr);
		}

		shared_proxy<T> access()
		{
			if (m_mutex)
			{
				return shared_proxy<T>(m_data, *m_mutex);
			}
			else
			{
				SPARK_FATAL("shared_shield_ptr does not have a mutex");
			}
		}

		i32 use_count() const
		{
			return m_data.use_count();
		}

	private:
		std::shared_ptr<T> m_data;
		std::shared_ptr<std::recursive_mutex> m_mutex;
	};

	template <typename T, typename... Args>
	unique_shield_ptr<T> make_unique_shield(Args&&... args)
	{
		return unique_shield_ptr<T>(new T(std::forward<Args>(args)...));
	}

	template <typename T, typename... Args>
	shared_shield_ptr<T> make_shared_shield(Args&&... args)
	{
		return shared_shield_ptr<T>(std::make_shared<T>(std::forward<Args>(args)...));
	}

	template <typename T>
	unique_shield_ptr<T> make_threaded(std::unique_ptr<T>&& ptr)
	{
		return unique_shield_ptr<T>(ptr.release());
	}

	template <typename T>
	shared_shield_ptr<T> make_threaded(std::shared_ptr<T> ptr)
	{
		return shared_shield_ptr<T>(std::move(ptr), std::make_shared<std::recursive_mutex>());
	}

	template <typename T, typename U>
	unique_shield_ptr<T> threaded_pointer_cast(const unique_shield_ptr<U>& ptr)
	{
		return unique_shield_ptr<T>((T*)ptr.get(), ptr.get_mutex());
	}

	template <typename T, typename U>
	shared_shield_ptr<T> threaded_pointer_cast(const shared_shield_ptr<U>& ptr)
	{
		auto casted_ptr = std::static_pointer_cast<T>(ptr.get_shared_ptr());
		return shared_shield_ptr<T>(std::move(casted_ptr), ptr.get_mutex());
	}
}

#endif