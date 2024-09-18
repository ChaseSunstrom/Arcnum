#ifndef SPARK_REF_PTR_HPP
#define SPARK_REF_PTR_HPP

#include <core/pch.hpp>
#include "shared_ptr.hpp"
#include "unique_ptr.hpp"
#include "weak_ptr.hpp"

namespace Spark {
	// Basically a non-owning reference to a pointer
	// useful for passing around pointers without taking ownership
	// of the pointer
	// This is good for when we want to just use a unique ptr for
	// cleaning up memory but want to also be able to store it somewhere else
	// without using a raw pointer or a shared ptr
	template<typename _Ty> class RefPtr {
	  public:
		RefPtr(const _Ty* ptr)
			: m_ptr(ptr) {}
		RefPtr(_Ty* ptr)
			: m_ptr(ptr) {}
		RefPtr(const _Ty& ref)
			: m_ptr(&ref) {}
		RefPtr(_Ty& ref)
			: m_ptr(&ref) {}
		RefPtr(std::nullptr_t)
			: m_ptr(nullptr) {}
		RefPtr()
			: m_ptr(nullptr) {}
		RefPtr(const RefPtr<_Ty>& other)
			: m_ptr(other.m_ptr) {}
		RefPtr(RefPtr<_Ty>&& other) noexcept
			: m_ptr(other.m_ptr) {}
		RefPtr(const std::unique_ptr<_Ty>& ptr)
			: m_ptr(ptr.get()) {}
		RefPtr(const std::shared_ptr<_Ty>& ptr)
			: m_ptr(ptr.get()) {}
		RefPtr(const std::weak_ptr<_Ty>& ptr)
			: m_ptr(ptr.lock().get()) {}
		RefPtr(const UniquePtr<_Ty>& ptr)
			: m_ptr(ptr.Get()) {}
		RefPtr(const SharedPtr<_Ty>& ptr)
			: m_ptr(ptr.Get()) {}
		RefPtr(const WeakPtr<_Ty>& ptr)
			: m_ptr(ptr.Get()) {}

		// Allow implicit conversion to the underlying pointer
		operator _Ty&() const { return *m_ptr; }
		operator _Ty*() const { return m_ptr; }

		RefPtr& operator=(const RefPtr<_Ty>& other) {
			m_ptr = other.m_ptr;
			return *this;
		}

		RefPtr& operator=(RefPtr<_Ty>&& other) noexcept {
			m_ptr = other.m_ptr;
			return *this;
		}

		RefPtr& operator=(const std::unique_ptr<_Ty>& ptr) {
			m_ptr = ptr.get();
			return *this;
		}

		RefPtr& operator=(const std::shared_ptr<_Ty>& ptr) {
			m_ptr = ptr.get();
			return *this;
		}

		RefPtr& operator=(const std::weak_ptr<_Ty>& ptr) {
			m_ptr = ptr.lock().get();
			return *this;
		}

		RefPtr& operator=(const UniquePtr<_Ty>& ptr) {
			m_ptr = ptr.Get();
			return *this;
		}

		RefPtr& operator=(const SharedPtr<_Ty>& ptr) {
			m_ptr = ptr.Get();
			return *this;
		}

		RefPtr& operator=(const WeakPtr<_Ty>& ptr) {
			m_ptr = ptr.Lock().Get();
			return *this;
		}

		RefPtr& operator=(const _Ty* ptr) {
			m_ptr = ptr;
			return *this;
		}

		RefPtr& operator=(_Ty* ptr) {
			m_ptr = ptr;
			return *this;
		}

		RefPtr& operator=(std::nullptr_t) {
			m_ptr = nullptr;
			return *this;
		}

		RefPtr& operator=(const _Ty& ptr) {
			m_ptr = &ptr;
			return *this;
		}

		RefPtr& operator=(const _Ty&& ptr) {
			m_ptr = &ptr;
			return *this;
		}

		bool operator!() const { return m_ptr == nullptr; }
		operator bool() const { return m_ptr != nullptr; }

		_Ty* operator->() const { return m_ptr; }
		_Ty& operator*() const { return *m_ptr; }

		bool operator==(const RefPtr<_Ty>& other) const { return m_ptr == other.m_ptr; }
		bool operator!=(const RefPtr<_Ty>& other) const { return m_ptr != other.m_ptr; }
		bool operator==(const _Ty* other) const { return m_ptr == other; }
		bool operator!=(const _Ty* other) const { return m_ptr != other; }
		bool operator==(std::nullptr_t) const { return m_ptr == nullptr; }
		bool operator!=(std::nullptr_t) const { return m_ptr != nullptr; }

		_Ty*       Get() const { return m_ptr; }
		const _Ty* GetConst() const { return m_ptr; }

	  private:
		_Ty* m_ptr;
	};

	// Deduction guide
	template<typename... Args> RefPtr(Args... args) -> RefPtr<std::common_type_t<Args...>>;

	template<typename _Ty, typename... Args> RefPtr<_Ty> MakeRefPtr(Args... args) { return RefPtr<_Ty>(args...); }

	template<typename _Ty> RefPtr<_Ty> MakeRefPtr(const _Ty* ptr) { return RefPtr<_Ty>(ptr); }

	template<typename _Ty> RefPtr<_Ty> MakeRefPtr(const _Ty& ref) { return RefPtr<_Ty>(&ref); }

	template<typename To, typename From> RefPtr<To> RefCast(const RefPtr<From>& from) { return RefPtr<To>(static_cast<To*>(const_cast<From*>(from.Get()))); }

	template<typename To, typename From> RefPtr<const To> RefCast(const RefPtr<const From>& from) { return RefPtr<const To>(dynamic_cast<const To*>(from.Get())); }
} // namespace Spark

#endif // SPARK_REF_PTR_HPP