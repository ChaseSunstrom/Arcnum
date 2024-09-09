#ifndef SPARK_WEAK_PTR_HPP
#define SPARK_WEAK_PTR_HPP

#include "shared_ptr.hpp"

namespace Spark {
	template<typename T> class WeakPtr {
	public:
		WeakPtr()
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr) {}

		WeakPtr(const SharedPtr<T>& shared_ptr)
			: m_ptr(shared_ptr.m_ptr)
			, m_ref_count(shared_ptr.m_ref_count)
			, m_weak_count(shared_ptr.m_weak_count) { if (m_weak_count) { m_weak_count->fetch_add(1); } }

		WeakPtr(std::nullptr_t)
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr) {}

		WeakPtr(const std::shared_ptr<T>& ptr)
			: m_ptr(ptr.get())
			, m_ref_count(ptr.use_count() ? new std::atomic<i32>(ptr.use_count()) : nullptr)
			, m_weak_count(ptr.use_count() ? new std::atomic<i32>(1) : nullptr) {}

		WeakPtr(const std::weak_ptr<T>& ptr)
			: m_ptr(ptr.lock().get())
			, m_ref_count(ptr.use_count() ? new std::atomic<i32>(ptr.use_count()) : nullptr)
			, m_weak_count(ptr.use_count() ? new std::atomic<i32>(1) : nullptr) {}

		WeakPtr(const WeakPtr& other)
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count) { if (m_weak_count) { m_weak_count->fetch_add(1); } }

		WeakPtr& operator=(const WeakPtr& other) {
			if (this != &other) {
				Reset();
				m_ptr        = other.m_ptr;
				m_ref_count  = other.m_ref_count;
				m_weak_count = other.m_weak_count;
				if (m_weak_count) { m_weak_count->fetch_add(1); }
			}
			return *this;
		}

		WeakPtr(WeakPtr&& other) noexcept
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count) {
			other.m_ptr        = nullptr;
			other.m_ref_count  = nullptr;
			other.m_weak_count = nullptr;
		}

		WeakPtr& operator=(WeakPtr&& other) noexcept {
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

		~WeakPtr() { Reset(); }

		void Reset() {
			if (m_weak_count) {
				if (m_weak_count->fetch_sub(1) == 1) {
					if (m_ref_count->load() == 0) {
						delete m_ref_count;
						delete m_weak_count;
					}
				}
				m_ptr        = nullptr;
				m_ref_count  = nullptr;
				m_weak_count = nullptr;
			}
		}

		SharedPtr<T> Lock() const {
			if (m_ref_count && m_ref_count->load() > 0) { return SharedPtr<T>(m_ptr, m_ref_count, m_weak_count); }
			return SharedPtr<T>(nullptr);
		}

		bool Expired() const { return m_ref_count == nullptr || m_ref_count->load() == 0; }

		i32 UseCount() const { return m_ref_count ? m_ref_count->load() : 0; }

		i32 WeakCount() const { return m_weak_count ? m_weak_count->load() : 0; }

	private:
		T*                m_ptr;
		std::atomic<i32>* m_ref_count;
		std::atomic<i32>* m_weak_count;

		friend class SharedPtr<T>;
	};
} // namespace Spark

#endif // SPARK_WEAK_PTR_HPP