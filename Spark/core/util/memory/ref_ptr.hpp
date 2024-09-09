#ifndef SPARK_REF_PTR_HPP
#define SPARK_REF_PTR_HPP

#include <core/pch.hpp>
#include "unique_ptr.hpp"
#include "shared_ptr.hpp"
#include "weak_ptr.hpp"

namespace Spark {
	// Basically a non-owning reference to a pointer
	// useful for passing around pointers without taking ownership
	// of the pointer
	// This is good for when we want to just use a unique ptr for
	// cleaning up memory but want to also be able to store it somewhere else
	// without using a raw pointer or a shared ptr
	template<typename T> class RefPtr {
	public:
		RefPtr(const T* ptr) : m_ptr(ptr) {}
		RefPtr(T* ptr) : m_ptr(ptr) {}
		RefPtr(const T& ref) : m_ptr(&ref) {}
		RefPtr(T& ref) : m_ptr(&ref) {}
		RefPtr(std::nullptr_t) : m_ptr(nullptr) {}
		RefPtr() : m_ptr(nullptr) {}
		RefPtr(const RefPtr<T>& other) : m_ptr(other.m_ptr) {}
		RefPtr(RefPtr<T>&& other) noexcept : m_ptr(other.m_ptr) {}
		RefPtr(const std::unique_ptr<T>& ptr) : m_ptr(ptr.get()) {}
		RefPtr(const std::shared_ptr<T>& ptr) : m_ptr(ptr.get()) {}
		RefPtr(const std::weak_ptr<T>& ptr) : m_ptr(ptr.lock().get()) {}
		RefPtr(const UniquePtr<T>& ptr) : m_ptr(ptr.Get()) {}
		RefPtr(const SharedPtr<T>& ptr) : m_ptr(ptr.Get()) {}
		RefPtr(const WeakPtr<T>& ptr) : m_ptr(ptr.Get()) {}
		
		// Allow implicit conversion to the underlying pointer
		operator T&() const { return *m_ptr; }
		operator T*() const { return m_ptr; }
		
		RefPtr& operator=(const RefPtr<T>& other) {
			m_ptr = other.m_ptr;
			return *this;
		}

		RefPtr& operator=(RefPtr<T>&& other) noexcept {
			m_ptr = other.m_ptr;
			return *this;
		}

		RefPtr& operator=(const std::unique_ptr<T>& ptr) {
			m_ptr = ptr.get();
			return *this;
		}

		RefPtr& operator=(const std::shared_ptr<T>& ptr) {
			m_ptr = ptr.get();
			return *this;
		}

		RefPtr& operator=(const std::weak_ptr<T>& ptr) {
			m_ptr = ptr.lock().get();
			return *this;
		}

		RefPtr& operator=(const T* ptr) {
			m_ptr = ptr;
			return *this;
		}

		RefPtr& operator=(T* ptr) {
			m_ptr = ptr;
			return *this;
		}


		RefPtr& operator=(std::nullptr_t) {
			m_ptr = nullptr;
			return *this;
		}

		RefPtr& operator=(const T& ptr) {
			m_ptr = &ptr;
			return *this;
		}

		RefPtr& operator=(const T&& ptr) {
			m_ptr = &ptr;
			return *this;
		}

		bool operator!() const { return m_ptr == nullptr; }
		operator bool() const { return m_ptr != nullptr; }

		T* operator->() const { return m_ptr; }
		T& operator*() const { return *m_ptr; }

		bool operator==(const RefPtr<T>& other) const { return m_ptr == other.m_ptr; }
		bool operator!=(const RefPtr<T>& other) const { return m_ptr != other.m_ptr; }
		bool operator==(const T* other) const { return m_ptr == other; }
		bool operator!=(const T* other) const { return m_ptr != other; }
		bool operator==(std::nullptr_t) const { return m_ptr == nullptr; }
		bool operator!=(std::nullptr_t) const { return m_ptr != nullptr; }

		T* Get() const { return m_ptr; }
		const T* GetConst() const { return m_ptr; }

	private:
		T* m_ptr;
	};

	// Deduction guide
	template<typename... Args> RefPtr(Args... args) -> RefPtr<std::common_type_t<Args...>>;

	template<typename T, typename... Args> RefPtr<T> MakeRefPtr(Args... args) { return RefPtr<T>(args...); }

	template<typename T> RefPtr<T> MakeRefPtr(const T* ptr) { return RefPtr<T>(ptr); }

	template<typename T> RefPtr<T> MakeRefPtr(const T& ref) { return RefPtr<T>(&ref); }

	template<typename To, typename From> RefPtr<To> RefCast(const RefPtr<From>& from) { return RefPtr<To>(static_cast<To*>(const_cast<From*>(from.Get()))); }

	template<typename To, typename From> RefPtr<const To> RefCast(const RefPtr<const From>& from) { return RefPtr<const To>(dynamic_cast<const To*>(from.Get())); }
}

#endif // SPARK_REF_PTR_HPP