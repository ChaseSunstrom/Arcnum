#ifndef SPARK_SHARED_PTR_HPP
#define SPARK_SHARED_PTR_HPP

#include <atomic>
#include <core/util/classic/traits.hpp>
#include <memory>
#include "allocator.hpp"

namespace Spark {
	template<typename _Ty, typename Allocator> class WeakPtr;

	/**
	 * @brief A smart pointer that retains shared ownership of an object through a pointer.
	 *
	 * SharedPtr is a reference-counting smart pointer that allows multiple pointers to refer
	 * to the same object. The object is destroyed when the last SharedPtr pointing to it is destroyed.
	 *
	 * @tparam _Ty The type of the managed object
	 * @tparam Allocator The allocator type to use for memory management
	 */
	template<typename _Ty, typename Allocator = _SPARK Allocator<_Ty>> class SharedPtr {
	  public:
		using AllocatorType   = Allocator;
		using AllocatorTraits = AllocatorTraits<AllocatorType>;

		using ValueType       = typename AllocatorTraits::ValueType;
		using Pointer         = typename AllocatorTraits::Pointer;
		using ConstPointer    = typename AllocatorTraits::ConstPointer;
		using Reference       = typename AllocatorTraits::Reference;
		using ConstReference  = typename AllocatorTraits::ConstReference;
		using SizeType        = typename AllocatorTraits::SizeType;
		using DifferenceType  = typename AllocatorTraits::DifferenceType;

		/**
		 * @brief Default constructor. Creates a null SharedPtr.
		 */
		SharedPtr() noexcept
			: m_ptr(nullptr)
			, m_ref_count(nullptr)
			, m_weak_count(nullptr) {}

		/**
		 * @brief Constructs a SharedPtr that owns nothing.
		 */
		SharedPtr(nullptr_t) noexcept
			: SharedPtr() {}

		/**
		 * @brief Constructs a SharedPtr from a std::shared_ptr.
		 * @param ptr The std::shared_ptr to construct from
		 */
		SharedPtr(const std::shared_ptr<_Ty>& ptr)
			: m_ptr(ptr.get())
			, m_ref_count(ptr.use_count() ? AllocatorTraits::Allocate(m_allocactor, 1) : nullptr)
			, m_weak_count(ptr.use_count() ? AllocatorTraits::Allocate(m_allocactor, 1) : nullptr) {
			if (m_ref_count) {
				AllocatorTraits::Construct(m_allocactor, m_ref_count, ptr.use_count());
				AllocatorTraits::Construct(m_allocactor, m_weak_count, 1);
			}
		}

		/**
		 * @brief Constructs a SharedPtr from a std::weak_ptr.
		 * @param ptr The std::weak_ptr to construct from
		 */
		SharedPtr(const std::weak_ptr<_Ty>& ptr)
			: SharedPtr(ptr.lock()) {}

		/**
		 * @brief Move constructor.
		 * @param other The SharedPtr to move from
		 */
		template<typename _Uty, typename = EnableIfT<IsConvertibleV<_Uty*, _Ty*>>> SharedPtr(SharedPtr<_Uty>&& other) noexcept
			: m_ptr(other.Release())
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count)
			, m_allocactor(Move(other.m_allocactor)) {
			other.m_ref_count  = nullptr;
			other.m_weak_count = nullptr;
		}

		/**
		 * @brief Constructs a SharedPtr that shares ownership with another SharedPtr but stores a different pointer.
		 * @param other The SharedPtr to share ownership with
		 * @param ptr The pointer to store
		 */
		template<typename _Uty> SharedPtr(const SharedPtr<_Uty>& other, _Ty* ptr) noexcept
			: m_ptr(ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count)
			, m_allocactor(other.m_allocactor) {
			if (m_ref_count) {
				m_ref_count->fetch_add(1, std::memory_order_relaxed);
			}
		}

		/**
		 * @brief Copy constructor.
		 * @param other The SharedPtr to copy from
		 */
		template<typename _Uty, typename = EnableIfT<IsConvertibleV<_Uty*, _Ty*>>> SharedPtr(const SharedPtr<_Uty>& other) noexcept
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count)
			, m_allocactor(other.m_allocactor) {
			if (m_ref_count) {
				m_ref_count->fetch_add(1, std::memory_order_relaxed);
			}
		}

		/**
		 * @brief Copy assignment operator.
		 * @param other The SharedPtr to copy from
		 * @return A reference to this SharedPtr
		 */
		template<typename _Uty, typename = EnableIfT<IsConvertibleV<_Uty*, _Ty*>>> SharedPtr& operator=(const SharedPtr<_Uty>& other) noexcept {
			if (this != reinterpret_cast<const SharedPtr*>(&other)) {
				Reset();
				m_ptr        = other.m_ptr;
				m_ref_count  = other.m_ref_count;
				m_weak_count = other.m_weak_count;
				m_allocactor = other.m_allocactor;
				if (m_ref_count) {
					m_ref_count->fetch_add(1, std::memory_order_relaxed);
				}
			}
			return *this;
		}

		/**
		 * @brief Constructs a SharedPtr that owns the pointer p.
		 * @param ptr Pointer to an object to manage
		 */
		explicit SharedPtr(_Ty* ptr)
			: m_ptr(ptr)
			, m_ref_count(ptr ? AllocatorTraits::Allocate(m_allocactor, 1) : nullptr)
			, m_weak_count(ptr ? AllocatorTraits::Allocate(m_allocactor, 1) : nullptr) {
			if (m_ref_count) {
				AllocatorTraits::Construct(m_allocactor, m_ref_count, 1);
				AllocatorTraits::Construct(m_allocactor, m_weak_count, 0);
			}
		}

		/**
		 * @brief Destructor. Decrements the reference count and deletes the managed object if it reaches zero.
		 */
		~SharedPtr() { Reset(); }

		/**
		 * @brief Copy constructor.
		 * @param other The SharedPtr to copy from
		 */
		SharedPtr(const SharedPtr& other)
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count)
			, m_allocactor(other.m_allocactor) {
			if (m_ref_count) {
				m_ref_count->fetch_add(1, std::memory_order_relaxed);
			}
		}

		/**
		 * @brief Copy assignment operator.
		 * @param other The SharedPtr to copy from
		 * @return A reference to this SharedPtr
		 */
		SharedPtr& operator=(const SharedPtr& other) {
			if (this != &other) {
				Reset();
				m_ptr        = other.m_ptr;
				m_ref_count  = other.m_ref_count;
				m_weak_count = other.m_weak_count;
				m_allocactor = other.m_allocactor;
				if (m_ref_count) {
					m_ref_count->fetch_add(1, std::memory_order_relaxed);
				}
			}
			return *this;
		}

		/**
		 * @brief Move constructor.
		 * @param other The SharedPtr to move from
		 */
		SharedPtr(SharedPtr&& other) noexcept
			: m_ptr(other.m_ptr)
			, m_ref_count(other.m_ref_count)
			, m_weak_count(other.m_weak_count)
			, m_allocactor(Move(other.m_allocactor)) {
			other.m_ptr        = nullptr;
			other.m_ref_count  = nullptr;
			other.m_weak_count = nullptr;
		}

		/**
		 * @brief Move assignment operator.
		 * @param other The SharedPtr to move from
		 * @return A reference to this SharedPtr
		 */
		SharedPtr& operator=(SharedPtr&& other) noexcept {
			if (this != &other) {
				Reset();
				m_ptr              = other.m_ptr;
				m_ref_count        = other.m_ref_count;
				m_weak_count       = other.m_weak_count;
				m_allocactor       = Move(other.m_allocactor);
				other.m_ptr        = nullptr;
				other.m_ref_count  = nullptr;
				other.m_weak_count = nullptr;
			}
			return *this;
		}

		/**
		 * @brief Checks if the SharedPtr is not null.
		 * @return true if the SharedPtr is not null, false otherwise
		 */
		explicit operator bool() const { return m_ptr != nullptr; }

		/**
		 * @brief Dereference operator.
		 * @return A reference to the managed object
		 */
		Reference operator*() const { return *m_ptr; }

		/**
		 * @brief Member access operator.
		 * @return A pointer to the managed object
		 */
		Pointer operator->() const { return m_ptr; }

		/**
		 * @brief Array subscript operator.
		 * @param index The index of the element to access
		 * @return A reference to the element at the specified index
		 */
		Reference operator[](i32 index) const { return m_ptr[index]; }

		/**
		 * @brief Gets the stored pointer.
		 * @return The stored pointer
		 */
		Pointer Get() const { return m_ptr; }

		/**
		 * @brief Releases ownership of the managed object.
		 * @return The stored pointer
		 */
		Pointer Release() {
			Pointer ptr = m_ptr;
			m_ptr       = nullptr;
			return ptr;
		}

		/**
		 * @brief Resets the SharedPtr to empty.
		 */
		void Reset() {
			if (m_ref_count) {
				if (m_ref_count->fetch_sub(1, std::memory_order_acq_rel) == 1) {
					AllocatorTraits::Destroy(m_allocactor, m_ptr);
					AllocatorTraits::Deallocate(m_allocactor, m_ptr, 1);
					if (m_weak_count->load(std::memory_order_acquire) == 0) {
						AllocatorTraits::Deallocate(m_allocactor, m_ref_count, 1);
						AllocatorTraits::Deallocate(m_allocactor, m_weak_count, 1);
					}
				}
				m_ptr        = nullptr;
				m_ref_count  = nullptr;
				m_weak_count = nullptr;
			}
		}

		/**
		 * @brief Gets the number of SharedPtr instances managing the current object.
		 * @return The number of SharedPtr instances
		 */
		i32 UseCount() const { return m_ref_count ? m_ref_count->load(std::memory_order_relaxed) : 0; }

		/**
		 * @brief Gets the number of weak references to the managed object.
		 * @return The number of weak references
		 */
		i32 WeakCount() const { return m_weak_count ? m_weak_count->load(std::memory_order_relaxed) : 0; }

	  private:
		/**
		 * @brief Constructor for use by WeakPtr::Lock()
		 */
		SharedPtr(Pointer ptr, std::atomic<i32>* ref_count, std::atomic<i32>* weak_count)
			: m_ptr(ptr)
			, m_ref_count(ref_count)
			, m_weak_count(weak_count) {
			if (m_ref_count) {
				m_ref_count->fetch_add(1, std::memory_order_relaxed);
			}
		}

	  private:
		Pointer           m_ptr;
		std::atomic<i32>* m_ref_count;
		std::atomic<i32>* m_weak_count;
		AllocatorType     m_allocactor;

		friend class WeakPtr<_Ty>;
		template<typename _Uty> friend class SharedPtr;
	};

	/**
	 * @brief Creates a SharedPtr to an object of type _Ty, using the provided allocator.
	 * @tparam _Ty The type of the object to create
	 * @tparam Args The types of the arguments to forward to the constructor of _Ty
	 * @param args The arguments to forward to the constructor of _Ty
	 * @return A SharedPtr to the newly created object
	 */
	template<typename _Ty, typename... Args> SharedPtr<_Ty> MakeShared(Args&&... args) { return SharedPtr<_Ty>(new _Ty(Forward<Args>(args)...)); }

	/**
	 * @brief Creates a SharedPtr to an object of type _Ty, using the provided allocator.
	 * @tparam _Ty The type of the object to create
	 * @param ptr The pointer to an object of type _Ty
	 * @return A SharedPtr to the object
	 */
	template<typename _Ty> SharedPtr<_Ty> MakeShared(const _Ty* ptr) { return SharedPtr<_Ty>(ptr); }

	/**
	 * @brief Creates a SharedPtr to an object of type _Ty, using the provided allocator.
	 * @tparam _Ty The type of the object to create
	 * @param ref The reference to an object of type _Ty
	 * @return A SharedPtr to the object
	 */
	template<typename _Ty> SharedPtr<_Ty> MakeShared(const _Ty& ref) { return SharedPtr<_Ty>(&ref); }

	/**
	 * @brief Performs a static pointer cast on the managed object.
	 * @tparam To The type to cast to
	 * @tparam From The type to cast from
	 * @param from The SharedPtr to cast from
	 * @return A SharedPtr to the casted object, or nullptr if the cast is not possible
	 */
	template<typename To, typename From> SharedPtr<To> StaticPointerCast(const SharedPtr<From>& from) {
		auto* ptr = static_cast<To*>(from.Get());
		return ptr ? SharedPtr<To>(from, ptr) : nullptr;
	}

	/**
	 * @brief Performs a dynamic pointer cast on the managed object.
	 * @tparam To The type to cast to
	 * @tparam From The type to cast from
	 * @param from The SharedPtr to cast from
	 * @return A SharedPtr to the casted object, or nullptr if the cast is not possible
	 */
	template<typename To, typename From> SharedPtr<To> DynamicPointerCast(const SharedPtr<From>& from) {
		auto* ptr = dynamic_cast<To*>(from.Get());
		return ptr ? SharedPtr<To>(from, ptr) : nullptr;
	}

	/**
	 * @brief Performs a reinterpret pointer cast on the managed object.
	 * @tparam To The type to cast to
	 * @tparam From The type to cast from
	 * @param from The SharedPtr to cast from
	 * @return A SharedPtr to the casted object, or nullptr if the cast is not possible
	 */
	template<typename To, typename From> SharedPtr<To> ReinterpretPointerCast(const SharedPtr<From>& from) {
		auto* ptr = reinterpret_cast<To*>(from.Get());
		return ptr ? SharedPtr<To>(from, ptr) : nullptr;
	}
} // namespace Spark

#endif // SPARK_SHARED_PTR_HPP