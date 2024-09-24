#ifndef SPARK_WEAK_PTR_HPP
#define SPARK_WEAK_PTR_HPP

#include "shared_ptr.hpp"

namespace Spark {
	
	/**
	 * @brief A weak pointer class that doesn't own the object it points to.
	 * 
	 * @tparam _Ty The type of the object being pointed to.
	 * @tparam Allocator The allocator type used for memory management.
	 */
	template<typename _Ty, typename Allocator = _SPARK Allocator<_Ty>>
	class WeakPtr {
	public:
		using AllocatorType      = Allocator;
		using AllocatorTraits    = AllocatorTraits<AllocatorType>;

		using ValueType          = typename AllocatorTraits::ValueType;
		using Pointer        = typename AllocatorTraits::Pointer;
		using Reference      = typename AllocatorTraits::Reference;
		using ConstReference = typename AllocatorTraits::ConstReference;
		using ConstPointer   = typename AllocatorTraits::ConstPointer;
		using DifferenceType     = typename AllocatorTraits::DifferenceType;
		using SizeType           = typename AllocatorTraits::SizeType;

		/**
		 * @brief Default constructor.
		 */
		WeakPtr()
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr)
			, m_allocator() {}

		/**
		 * @brief Constructor from SharedPtr.
		 * @param shared_ptr The SharedPtr to create a weak reference from.
		 */
		WeakPtr(const SharedPtr<_Ty, Allocator>& shared_ptr)
			: m_ptr(shared_ptr.m_ptr)
			, m_ref_count(shared_ptr.m_ref_count)
			, m_weak_count(shared_ptr.m_weak_count)
			, m_allocator(shared_ptr.GetAllocator()) {
			if (m_weak_count) {
				m_weak_count->fetch_add(1);
			}
		}

		/**
		 * @brief Constructor from std::shared_ptr.
		 * @param shared_ptr The std::shared_ptr to create a weak reference from.
		 */
		WeakPtr(const std::shared_ptr<_Ty>& shared_ptr)
			: WeakPtr(SharedPtr<_Ty, Allocator>(shared_ptr)) {}

		/**
		 * @brief Constructor from std::weak_ptr.
		 * @param weak_ptr The std::weak_ptr to create a weak reference from.
		 */
		WeakPtr(const std::weak_ptr<_Ty>& weak_ptr)
			: WeakPtr(SharedPtr<_Ty, Allocator>(weak_ptr)) {}

		/**
		 * @brief Constructor from nullptr.
		 */
		WeakPtr(nullptr_t)
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr)
			, m_allocator() {}

		/**
		 * @brief Copy constructor.
		 * @param other The WeakPtr to copy from.
		 */
		WeakPtr(const WeakPtr& other)
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count)
			, m_allocator(other.m_allocator) {
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
				m_allocator  = other.m_allocator;
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
			, m_weak_count(other.m_weak_count)
			, m_allocator(Move(other.m_allocator)) {
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
				m_allocator        = Move(other.m_allocator);
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
		WeakPtr& operator=(const SharedPtr<_Ty, Allocator>& other) {
			Reset();
			m_ptr = other.Get();
			m_ref_count = other.m_ref_count;
			m_weak_count = other.m_weak_count;
			m_allocator = other.GetAllocator();
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
				m_ref_count = AllocatorTraits::Allocate(m_allocator, 1);
				m_weak_count = AllocatorTraits::Allocate(m_allocator, 1);
				AllocatorTraits::Construct(m_allocator, m_ref_count, other.use_count());
				AllocatorTraits::Construct(m_allocator, m_weak_count, 1);
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
				m_ref_count = AllocatorTraits::Allocate(m_allocator, 1);
				m_weak_count = AllocatorTraits::Allocate(m_allocator, 1);
				AllocatorTraits::Construct(m_allocator, m_ref_count, shared.use_count());
				AllocatorTraits::Construct(m_allocator, m_weak_count, other.use_count());
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
						AllocatorTraits::Deallocate(m_allocator, m_ref_count, 1);
						AllocatorTraits::Deallocate(m_allocator, m_weak_count, 1);
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
		SharedPtr<_Ty, AllocatorType> Lock() const {
			if (m_ref_count && m_ref_count->load() > 0) {
				return SharedPtr<_Ty, AllocatorType>(m_ptr, m_ref_count, m_weak_count, m_allocator);
			}
			return SharedPtr<_Ty, AllocatorType>(nullptr);
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

		/**
		 * @brief Gets the allocator associated with this WeakPtr.
		 * @return The allocator.
		 */
		AllocatorType GetAllocator() const { return m_allocator; }

	private:
		Pointer        m_ptr;
		std::atomic<i32>*  m_ref_count;
		std::atomic<i32>*  m_weak_count;
		AllocatorType      m_allocator;

		friend class SharedPtr< _Ty, AllocatorType>;
	};
} // namespace Spark

#endif // SPARK_WEAK_PTR_HPP