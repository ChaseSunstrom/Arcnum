#ifndef SPARK_UNIQUE_PTR_HPP
#define SPARK_UNIQUE_PTR_HPP

#include <core/util/classic/util.hpp>
#include <memory>

namespace Spark {

	// UniquePtr for single objects
	template<typename _Ty> class UniquePtr {
	  public:
		UniquePtr(_Ty* ptr = nullptr)
			: m_ptr(ptr) {}
		UniquePtr(const UniquePtr&)            = delete;
		UniquePtr& operator=(const UniquePtr&) = delete;
		UniquePtr(UniquePtr&& other) noexcept
			: m_ptr(other.m_ptr) {
			other.m_ptr = nullptr;
		}

		template<typename _Uty, typename = std::enable_if_t<std::is_convertible_v<_Uty*, _Ty*>>> UniquePtr(UniquePtr<_Uty>&& other) noexcept
			: m_ptr(other.Release()) {}
		UniquePtr(std::unique_ptr<_Ty>&& other) noexcept
			: m_ptr(other.release()) {}
		UniquePtr& operator=(UniquePtr&& other) noexcept {
			if (this != &other) {
				Reset();
				m_ptr       = other.m_ptr;
				other.m_ptr = nullptr;
			}
			return *this;
		}
		UniquePtr& operator=(std::unique_ptr<_Ty>&& other) noexcept {
			Reset(other.release());
			return *this;
		}

		~UniquePtr() { delete m_ptr; }

		_Ty* Get() const { return m_ptr; }
		_Ty* Release() {
			_Ty* ptr = m_ptr;
			m_ptr  = nullptr;
			return ptr;
		}
		void Reset(_Ty* ptr = nullptr) {
			if (m_ptr != ptr) {
				delete m_ptr;
				m_ptr = ptr;
			}
		}
		_Ty&       operator*() const { return *m_ptr; }
		_Ty*       operator->() const { return m_ptr; }
		explicit operator bool() const { return m_ptr != nullptr; }

	  private:
		_Ty* m_ptr;
	};

	// UniquePtr specialization for arrays
	template<typename _Ty> class UniquePtr<_Ty[]> {
	  public:
		UniquePtr(_Ty* ptr = nullptr)
			: m_ptr(ptr) {}
		UniquePtr(const UniquePtr&)            = delete;
		UniquePtr& operator=(const UniquePtr&) = delete;
		UniquePtr(UniquePtr&& other) noexcept
			: m_ptr(other.m_ptr) {
			other.m_ptr = nullptr;
		}

		template<typename _Uty, typename = std::enable_if_t<std::is_convertible_v<_Uty*, _Ty*>>> UniquePtr(UniquePtr<_Uty>&& other) noexcept
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

		_Ty* Get() const { return m_ptr; }
		_Ty* Release() {
			_Ty* ptr = m_ptr;
			m_ptr  = nullptr;
			return ptr;
		}
		void Reset(_Ty* ptr = nullptr) {
			if (m_ptr != ptr) {
				delete[] m_ptr;
				m_ptr = ptr;
			}
		}
		_Ty&       operator[](size_t  i) const { return m_ptr[i]; }
		explicit operator bool() const { return m_ptr != nullptr; }

	  private:
		_Ty* m_ptr;
	};

	// MakeUnique for single objects
	template<typename _Ty, typename... Args> UniquePtr<_Ty> MakeUnique(Args&&... args) { return UniquePtr<_Ty>(new _Ty(Forward<Args>(args)...)); }

	// MakeUniqueArray for arrays
	template<typename _Ty> UniquePtr<_Ty[]> MakeUniqueArray(size_t  size) { return UniquePtr<_Ty[]>(new _Ty[size]); }

	template<typename _Ty> UniquePtr<_Ty[]> MakeUniqueArray(size_t  size, const _Ty& value) {
		_Ty* ptr = new _Ty[size];
		for (size_t  i = 0; i < size; ++i) {
			ptr[i] = value;
		}
		return UniquePtr<_Ty[]>(ptr);
	}

	template<typename _Ty> UniquePtr<_Ty[]> MakeUniqueArray(size_t  size, _Ty&& value) {
		_Ty* ptr = new _Ty[size];
		for (size_t  i = 0; i < size; ++i) {
			ptr[i] = Move(value);
		}
		return UniquePtr<_Ty[]>(ptr);
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
