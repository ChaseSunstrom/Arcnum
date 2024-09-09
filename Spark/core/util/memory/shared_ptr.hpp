#ifndef SPARK_SHARED_PTR_HPP
#define SPARK_SHARED_PTR_HPP

#include <memory>
#include <atomic>

namespace Spark {
	template<typename T> class WeakPtr;

	template<typename T> class SharedPtr {
	public:
		SharedPtr()
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr) {}

		SharedPtr(std::nullptr_t)
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr) {}

		SharedPtr(const std::shared_ptr<T>& ptr)
			: m_ptr(ptr.get())
			, m_ref_count(ptr.use_count() ? new std::atomic<i32>(ptr.use_count()) : nullptr)
			, m_weak_count(ptr.use_count() ? new std::atomic<i32>(1) : nullptr) {}

		SharedPtr(const std::weak_ptr<T>& ptr)
			: m_ptr(ptr.lock().get())
			, m_ref_count(ptr.use_count() ? new std::atomic<i32>(ptr.use_count()) : nullptr)
			, m_weak_count(ptr.use_count() ? new std::atomic<i32>(1) : nullptr) {}

		explicit SharedPtr(T* ptr)
			: m_ptr(ptr)
			, m_ref_count(ptr ? new std::atomic<i32>(1) : nullptr)
			, m_weak_count(ptr ? new std::atomic<i32>(0) : nullptr) {}
		
		~SharedPtr() { Reset(); }

		SharedPtr(const SharedPtr& other)
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count) { if (m_ref_count) { m_ref_count->fetch_add(1); } }

		SharedPtr& operator=(const SharedPtr& other) {
			if (this != &other) {
				Reset();
				m_ptr        = other.m_ptr;
				m_ref_count  = other.m_ref_count;
				m_weak_count = other.m_weak_count;
				if (m_ref_count) { m_ref_count->fetch_add(1); }
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

		T& operator*() const { return *m_ptr; }
		T* operator->() const { return m_ptr; }
		T& operator[](i32 index) const { return m_ptr[index]; }

		T* Get() const { return m_ptr; }

		T* Release() {
			T* ptr = m_ptr;
			m_ptr  = nullptr;
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
		T*                m_ptr;
		std::atomic<i32>* m_ref_count;
		std::atomic<i32>* m_weak_count;

		// Constructor for use by WeakPtr::Lock()
		SharedPtr(T* ptr, std::atomic<i32>* ref_count, std::atomic<i32>* weak_count)
			: m_ptr(ptr)
			, m_ref_count(ref_count)
			, m_weak_count(weak_count) { if (m_ref_count) { m_ref_count->fetch_add(1); } }

		friend class WeakPtr<T>;
	};

	template <typename T, typename... Args> SharedPtr<T> MakeShared(Args&&... args) {
		return SharedPtr<T>(new T(std::forward<Args>(args)...));
	}

	template <typename T> SharedPtr<T> MakeShared(const T* ptr) {
		return SharedPtr<T>(ptr);
	}

	template <typename T> SharedPtr<T> MakeShared(const T& ref) {
		return SharedPtr<T>(&ref);
	}
}

#endif // SPARK_SHARED_PTR_HPP