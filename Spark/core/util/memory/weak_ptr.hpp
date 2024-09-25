#ifndef SPARK_WEAK_PTR_HPP
#define SPARK_WEAK_PTR_HPP

#include "shared_ptr.hpp"

namespace Spark {
	
	/**
	 * @brief A weak pointer class that doesn't own the object it points to.
	 * 
	 * @tparam _Ty The type of the object being pointed to.
	 */
	template<typename _Ty>
	class WeakPtr {
	public:
		using ValueType      = _Ty;
		using Pointer        = _Ty*;
		using Reference      = _Ty&;
		using ConstReference = const _Ty&;
		using ConstPointer   = const _Ty*;

		/**
		 * @brief Default constructor.
		 */
		WeakPtr()
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr) {}

		/**
		 * @brief Constructor from SharedPtr.
		 * @param shared_ptr The SharedPtr to create a weak reference from.
		 */
		WeakPtr(const SharedPtr<_Ty>& shared_ptr)
			: m_ptr(shared_ptr.m_ptr)
			, m_ref_count(shared_ptr.m_ref_count)
			, m_weak_count(shared_ptr.m_weak_count) {
			if (m_weak_count) {
				m_weak_count->fetch_add(1);
			}
		}

		/**
		 * @brief Constructor from std::shared_ptr.
		 * @param shared_ptr The std::shared_ptr to create a weak reference from.
		 */
		WeakPtr(const std::shared_ptr<_Ty>& shared_ptr)
			: WeakPtr(SharedPtr<_Ty>(shared_ptr)) {}

		/**
		 * @brief Constructor from std::weak_ptr.
		 * @param weak_ptr The std::weak_ptr to create a weak reference from.
		 */
		WeakPtr(const std::weak_ptr<_Ty>& weak_ptr)
			: WeakPtr(SharedPtr<_Ty>(weak_ptr.lock())) {}

		/**
		 * @brief Constructor from nullptr.
		 */
		WeakPtr(nullptr_t)
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr) {}

		/**
		 * @brief Copy constructor.
		 * @param other The WeakPtr to copy from.
		 */
		WeakPtr(const WeakPtr& other)
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count) {
			if (m_weak_count) {
				m_weak_count->fetch_add(1);
			}
		}

		/**
		 * @brief Copy assignment operator.
		 * @param other The WeakPtr to copy from.
		 * @return Reference to this WeakPtr.
		 */
		WeakPtr& operator=(const WeakPtr& other) {
			if (this != &other) {
				Reset();
				m_ptr        = other.m_ptr;
				m_ref_count  = other.m_ref_count;
				m_weak_count = other.m_weak_count;
				if (m_weak_count) {
					m_weak_count->fetch_add(1);
				}
			}
			return *this;
		}

		/**
		 * @brief Move constructor.
		 * @param other The WeakPtr to move from.
		 */
		WeakPtr(WeakPtr&& other) noexcept
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count) {
			other.m_ptr        = nullptr;
			other.m_ref_count  = nullptr;
			other.m_weak_count = nullptr;
		}

		/**
		 * @brief Move assignment operator.
		 * @param other The WeakPtr to move from.
		 * @return Reference to this WeakPtr.
		 */
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

		/**
		 * @brief Assignment operator from SharedPtr.
		 * @param other The SharedPtr to assign from.
		 * @return Reference to this WeakPtr.
		 */
		WeakPtr& operator=(const SharedPtr<_Ty>& other) {
			Reset();
			m_ptr = other.Get();
			m_ref_count = other.m_ref_count;
			m_weak_count = other.m_weak_count;
			if (m_weak_count) {
				m_weak_count->fetch_add(1);
			}
			return *this;
		}

		/**
		 * @brief Assignment operator from std::shared_ptr.
		 * @param other The std::shared_ptr to assign from.
		 * @return Reference to this WeakPtr.
		 */
		WeakPtr& operator=(const std::shared_ptr<_Ty>& other) {
			Reset();
			m_ptr = other.get();
			if (m_ptr) {
				m_ref_count = new std::atomic<i32>(other.use_count());
				m_weak_count = new std::atomic<i32>(1);
			}
			return *this;
		}

		/**
		 * @brief Assignment operator from std::weak_ptr.
		 * @param other The std::weak_ptr to assign from.
		 * @return Reference to this WeakPtr.
		 */
		WeakPtr& operator=(const std::weak_ptr<_Ty>& other) {
			Reset();
			if (!other.expired()) {
				auto shared = other.lock();
				m_ptr = shared.get();
				m_ref_count = new std::atomic<i32>(shared.use_count());
				m_weak_count = new std::atomic<i32>(other.use_count());
			}
			return *this;
		}

		/**
		 * @brief Destructor.
		 */
		~WeakPtr() { Reset(); }

		/**
		 * @brief Resets the WeakPtr to not point to any object.
		 */
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

		/**
		 * @brief Attempts to create a SharedPtr from this WeakPtr.
		 * @return A SharedPtr if the object still exists, otherwise a null SharedPtr.
		 */
		SharedPtr<_Ty> Lock() const {
			if (m_ref_count && m_ref_count->load() > 0) {
				return SharedPtr<_Ty>(m_ptr, m_ref_count, m_weak_count);
			}
			return SharedPtr<_Ty>(nullptr);
		}

		/**
		 * @brief Checks if the WeakPtr has expired (i.e., the object it points to has been deleted).
		 * @return true if expired, false otherwise.
		 */
		bool Expired() const { return m_ref_count == nullptr || m_ref_count->load() == 0; }

		/**
		 * @brief Gets the number of SharedPtr instances managing the object.
		 * @return The use count.
		 */
		i32 UseCount() const { return m_ref_count ? m_ref_count->load() : 0; }

		/**
		 * @brief Gets the number of WeakPtr instances referencing the object.
		 * @return The weak count.
		 */
		i32 WeakCount() const { return m_weak_count ? m_weak_count->load() : 0; }

	private:
		Pointer             m_ptr;
		std::atomic<i32>*   m_ref_count;
		std::atomic<i32>*   m_weak_count;

		friend class SharedPtr<_Ty>;
	};
} // namespace Spark

#endif // SPARK_WEAK_PTR_HPP