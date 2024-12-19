#ifndef SPARK_SHARED_PTR_HPP
#define SPARK_SHARED_PTR_HPP

#include <atomic>
#include <memory>

namespace Spark {
	template<typename _Ty> class WeakPtr;

	template<typename _Ty> class SharedPtr {
	  public:
		SharedPtr()
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr) {}

		SharedPtr(std::nullptr_t)
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr) {}

		SharedPtr(const std::shared_ptr<_Ty>& ptr)
			: m_ptr(ptr.get())
			, m_ref_count(ptr.use_count() ? new std::atomic<i32>(ptr.use_count()) : nullptr)
			, m_weak_count(ptr.use_count() ? new std::atomic<i32>(1) : nullptr) {}

		SharedPtr(const std::weak_ptr<_Ty>& ptr)
			: m_ptr(ptr.lock().get())
			, m_ref_count(ptr.use_count() ? new std::atomic<i32>(ptr.use_count()) : nullptr)
			, m_weak_count(ptr.use_count() ? new std::atomic<i32>(1) : nullptr) {}

		template<typename _Uty, typename = std::enable_if_t<std::is_convertible_v<_Uty*, _Ty*>>> SharedPtr(SharedPtr<_Uty>&& other) noexcept
			: m_ptr(other.Release()) {}

		template<typename _Uty> SharedPtr(const SharedPtr<_Uty>& other, _Ty* ptr) noexcept
			: m_ptr(ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count) {
			if (m_ref_count) {
				m_ref_count->fetch_add(1);
			}
		}

		template<typename _Uty, typename = std::enable_if_t<std::is_convertible_v<_Uty*, _Ty*>>> SharedPtr(const SharedPtr<_Uty>& other) noexcept
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count) {
			if (m_ref_count) {
				m_ref_count->fetch_add(1);
			}
		}

		template<typename _Uty, typename = std::enable_if_t<std::is_convertible_v<_Uty*, _Ty*>>> SharedPtr& operator=(const SharedPtr<_Uty>& other) noexcept {
			if (this != reinterpret_cast<const SharedPtr*>(&other)) {
				Reset();
				m_ptr        = other.m_ptr;
				m_ref_count  = other.m_ref_count;
				m_weak_count = other.m_weak_count;
				if (m_ref_count) {
					m_ref_count->fetch_add(1);
				}
			}
			return *this;
		}

		explicit SharedPtr(_Ty* ptr)
			: m_ptr(ptr)
			, m_ref_count(ptr ? new std::atomic<i32>(1) : nullptr)
			, m_weak_count(ptr ? new std::atomic<i32>(0) : nullptr) {}

		~SharedPtr() { Reset(); }

		SharedPtr(const SharedPtr& other)
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count) {
			if (m_ref_count) {
				m_ref_count->fetch_add(1);
			}
		}

		SharedPtr& operator=(const SharedPtr& other) {
			if (this != &other) {
				Reset();
				m_ptr        = other.m_ptr;
				m_ref_count  = other.m_ref_count;
				m_weak_count = other.m_weak_count;
				if (m_ref_count) {
					m_ref_count->fetch_add(1);
				}
			}
			return *this;
		}

		SharedPtr(SharedPtr&& other) noexcept
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count) {
			other.m_ptr        = nullptr;
			other.m_ref_count  = nullptr;
			other.m_weak_count = nullptr;
		}

		SharedPtr& operator=(SharedPtr&& other) noexcept {
			if (this != &other) {
				Reset();
				m_ptr              = other.m_ptr;
				m_ref_count        = other.m_ref_count;
				m_weak_count       = other.m_weak_count;
				other.m_ptr        = nullptr;
				other.m_ref_count  = nullptr;
				other.m_weak_count = nullptr;
			}
			return *this;
		}

		operator bool() const { return m_ptr != nullptr; }

		_Ty& operator*() const { return *m_ptr; }
		_Ty* operator->() const { return m_ptr; }
		_Ty& operator[](i32 index) const { return m_ptr[index]; }

		_Ty* Get() const { return m_ptr; }

		_Ty* Release() {
			_Ty* ptr = m_ptr;
			m_ptr    = nullptr;
			return ptr;
		}

		void Reset() {
			if (m_ref_count) {
				if (m_ref_count->fetch_sub(1) == 1) {
					delete m_ptr;
					if (m_weak_count->load() == 0) {
						delete m_ref_count;
						delete m_weak_count;
					}
				}
				m_ptr        = nullptr;
				m_ref_count  = nullptr;
				m_weak_count = nullptr;
			}
		}

		i32 UseCount() const { return m_ref_count ? m_ref_count->load() : 0; }

		i32 WeakCount() const { return m_weak_count ? m_weak_count->load() : 0; }

	  private:
		_Ty*              m_ptr;
		std::atomic<i32>* m_ref_count;
		std::atomic<i32>* m_weak_count;

		// Constructor for use by WeakPtr::Lock()
		SharedPtr(_Ty* ptr, std::atomic<i32>* ref_count, std::atomic<i32>* weak_count)
			: m_ptr(ptr)
			, m_ref_count(ref_count)
			, m_weak_count(weak_count) {
			if (m_ref_count) {
				m_ref_count->fetch_add(1);
			}
		}

		friend class WeakPtr<_Ty>;
		template<typename _Uty> friend class SharedPtr;
	};

	template<typename _Ty, typename... Args> SharedPtr<_Ty> MakeShared(Args&&... args) { return SharedPtr<_Ty>(new _Ty(Forward<Args>(args)...)); }

	template<typename _Ty> SharedPtr<_Ty> MakeShared(const _Ty* ptr) { return SharedPtr<_Ty>(ptr); }

	template<typename _Ty> SharedPtr<_Ty> MakeShared(const _Ty& ref) { return SharedPtr<_Ty>(&ref); }

	template<typename To, typename From> SharedPtr<To> StaticPointerCast(const SharedPtr<From>& from) {
		auto* ptr = static_cast<To*>(from.Get());
		return ptr ? SharedPtr<To>(from, ptr) : nullptr;
	}

	template<typename To, typename From> SharedPtr<To> DynamicPointerCast(const SharedPtr<From>& from) {
		auto* ptr = dynamic_cast<To*>(from.Get());
		return ptr ? SharedPtr<To>(from, ptr) : nullptr;
	}

	template<typename To, typename From> SharedPtr<To> ReinterpretPointerCast(const SharedPtr<From>& from) {
		auto* ptr = reinterpret_cast<To*>(from.Get());
		return ptr ? SharedPtr<To>(from, ptr) : nullptr;
	}
} // namespace Spark

#endif // SPARK_SHARED_PTR_HPP