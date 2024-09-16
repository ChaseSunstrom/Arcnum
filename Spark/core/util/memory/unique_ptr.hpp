#ifndef SPARK_UNIQUE_PTR_HPP
#define SPARK_UNIQUE_PTR_HPP

#include <core/util/classic/util.hpp>
#include <memory>

namespace Spark {

	// UniquePtr for single objects
	template<typename T> class UniquePtr {
	  public:
		UniquePtr(T* ptr = nullptr)
			: m_ptr(ptr) {}
		UniquePtr(const UniquePtr&)            = delete;
		UniquePtr& operator=(const UniquePtr&) = delete;
		UniquePtr(UniquePtr&& other) noexcept
			: m_ptr(other.m_ptr) {
			other.m_ptr = nullptr;
		}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>> UniquePtr(UniquePtr<U>&& other) noexcept
			: m_ptr(other.Release()) {}
		UniquePtr(std::unique_ptr<T>&& other) noexcept
			: m_ptr(other.release()) {}
		UniquePtr& operator=(UniquePtr&& other) noexcept {
			if (this != &other) {
				Reset();
				m_ptr       = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}
		UniquePtr& operator=(std::unique_ptr<T>&& other) noexcept {
			Reset(other.release());
			return *this;
		}

		~UniquePtr() { delete m_ptr; }

		T* Get() const { return m_ptr; }
		T* Release() {
			T* ptr = m_ptr;
			m_ptr  = nullptr;
			return ptr;
		}
		void Reset(T* ptr = nullptr) {
			if (m_ptr != ptr) {
				delete m_ptr;
				m_ptr = ptr;
			}
		}
		T&       operator*() const { return *m_ptr; }
		T*       operator->() const { return m_ptr; }
		explicit operator bool() const { return m_ptr != nullptr; }

	  private:
		T* m_ptr;
	};

	// UniquePtr specialization for arrays
	template<typename T> class UniquePtr<T[]> {
	  public:
		UniquePtr(T* ptr = nullptr)
			: m_ptr(ptr) {}
		UniquePtr(const UniquePtr&)            = delete;
		UniquePtr& operator=(const UniquePtr&) = delete;
		UniquePtr(UniquePtr&& other) noexcept
			: m_ptr(other.m_ptr) {
			other.m_ptr = nullptr;
		}

		template<typename U, typename = std::enable_if_t<std::is_convertible_v<U*, T*>>> UniquePtr(UniquePtr<U>&& other) noexcept
			: m_ptr(other.Release()) {}

		UniquePtr& operator=(UniquePtr&& other) noexcept {
			if (this != &other) {
				Reset();
				m_ptr       = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}
		~UniquePtr() { delete[] m_ptr; }

		T* Get() const { return m_ptr; }
		T* Release() {
			T* ptr = m_ptr;
			m_ptr  = nullptr;
			return ptr;
		}
		void Reset(T* ptr = nullptr) {
			if (m_ptr != ptr) {
				delete[] m_ptr;
				m_ptr = ptr;
			}
		}
		T&       operator[](size_t i) const { return m_ptr[i]; }
		explicit operator bool() const { return m_ptr != nullptr; }

	  private:
		T* m_ptr;
	};

	// MakeUnique for single objects
	template<typename T, typename... Args> UniquePtr<T> MakeUnique(Args&&... args) { return UniquePtr<T>(new T(Forward<Args>(args)...)); }

	// MakeUniqueArray for arrays
	template<typename T> UniquePtr<T[]> MakeUniqueArray(size_t size) { return UniquePtr<T[]>(new T[size]); }

	template<typename T> UniquePtr<T[]> MakeUniqueArray(size_t size, const T& value) {
		T* ptr = new T[size];
		for (size_t i = 0; i < size; ++i) {
			ptr[i] = value;
		}
		return UniquePtr<T[]>(ptr);
	}

	template<typename T> UniquePtr<T[]> MakeUniqueArray(size_t size, T&& value) {
		T* ptr = new T[size];
		for (size_t i = 0; i < size; ++i) {
			ptr[i] = Move(value);
		}
		return UniquePtr<T[]>(ptr);
	}

	// Pointer casts for single objects
	template<typename To, typename From> UniquePtr<To> StaticPointerCast(UniquePtr<From>&& from) {
		To* casted = static_cast<To*>(from.Get());
		from.Release();
		return UniquePtr<To>(casted);
	}

	template<typename To, typename From> UniquePtr<To> DynamicPointerCast(UniquePtr<From>&& from) {
		To* casted = dynamic_cast<To*>(from.Get());
		if (casted) {
			from.Release();
			return UniquePtr<To>(casted);
		}
		return UniquePtr<To>(nullptr);
	}

	template<typename To, typename From> UniquePtr<To> ReinterpretPointerCast(UniquePtr<From>&& from) {
		To* casted = reinterpret_cast<To*>(from.Get());
		from.Release();
		return UniquePtr<To>(casted);
	}

} // namespace Spark

#endif
