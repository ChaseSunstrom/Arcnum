#ifndef SPARK_UNIQUE_PTR_HPP
#define SPARK_UNIQUE_PTR_HPP

#include <memory>

namespace Spark {
	template <typename T> class UniquePtr {
	public:
		UniquePtr(T* ptr) : m_ptr(ptr) {}
		UniquePtr(std::nullptr_t) : m_ptr(nullptr) {}
		UniquePtr() : m_ptr(nullptr) {}
		UniquePtr(const std::unique_ptr<T>& ptr) : m_ptr(ptr.get()) {}
		UniquePtr(const std::shared_ptr<T>& ptr) : m_ptr(ptr.get()) {}
		
		~UniquePtr() {
			delete m_ptr;
		}

		UniquePtr(const UniquePtr&) = delete;
		UniquePtr& operator=(const UniquePtr&) = delete;

		UniquePtr(UniquePtr&& other) noexcept {
			m_ptr = other.m_ptr;
			other.m_ptr = nullptr;
		}

		UniquePtr& operator=(UniquePtr&& other) noexcept {
			if (this != &other) {
				delete m_ptr;
				m_ptr = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}

		operator bool() const {
			return m_ptr != nullptr;
		}

		T& operator*() const {
			return *m_ptr;
		}

		T* operator->() const {
			return m_ptr;
		}

		T& operator[](i32 index) const {
			return m_ptr[index];
		}

		T& Get() const {
			return *m_ptr;
		}

		T* Release() {
			T* ptr = m_ptr;
			m_ptr = nullptr;
			return ptr;
		}
	private:
		T* m_ptr;
	};

	template <typename T, typename... Args> UniquePtr<T> MakeUnique(Args&&... args) {
		return UniquePtr<T>(new T(std::forward<Args>(args)...));
	}

	template <typename T> UniquePtr<T> MakeUniqueArray(i32 size) {
		return UniquePtr<T>(new T[size]);
	}

	template <typename T> UniquePtr<T> MakeUniqueArray(i32 size, const T& value) {
		T* ptr = new T[size];
		for (i32 i = 0; i < size; ++i) {
			ptr[i] = value;
		}
		return UniquePtr<T>(ptr);
	}

	template <typename T> UniquePtr<T> MakeUniqueArray(i32 size, T&& value) {
		T* ptr = new T[size];
		for (i32 i = 0; i < size; ++i) {
			ptr[i] = value;
		}
		return UniquePtr<T>(ptr);
	}
}


#endif