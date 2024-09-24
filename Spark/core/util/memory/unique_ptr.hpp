#ifndef SPARK_UNIQUE_PTR_HPP
#define SPARK_UNIQUE_PTR_HPP

#include <core/util/classic/traits.hpp>
#include <core/util/classic/util.hpp>
#include <memory>
#include "allocator.hpp"

namespace Spark {
	
	/**
	 * @brief A smart pointer that owns and manages another object through a pointer and disposes of that object when the UniquePtr goes out of scope.
	 * 
	 * @tparam _Ty The type of the managed object.
	 * @tparam Allocator The allocator type used for memory management.
	 */
	template<typename _Ty, typename Allocator = Allocator<_Ty>>
	class UniquePtr {
	public:
		using AllocatorType      = Allocator;
		using AllocatorTraits    = AllocatorTraits<AllocatorType>;

		using ValueType          = typename AllocatorTraits::ValueType;
		using PointerType        = typename AllocatorTraits::Pointer;
		using ReferenceType      = typename AllocatorTraits::Reference;
		using ConstReferenceType = typename AllocatorTraits::ConstReference;
		using ConstPointerType   = typename AllocatorTraits::ConstPointer;
		using DifferenceType     = typename AllocatorTraits::DifferenceType;
		using SizeType           = typename AllocatorTraits::SizeType;

		/**
		 * @brief Constructs a UniquePtr that owns nothing.
		 */
		UniquePtr() : m_ptr(nullptr), m_allocator() {}

		/**
		 * @brief Constructs a UniquePtr with the given pointer.
		 * @param ptr Pointer to the object to manage.
		 */
		explicit UniquePtr(PointerType ptr) : m_ptr(ptr), m_allocator() {}

		/**
		 * @brief Copy constructor (deleted).
		 */
		UniquePtr(const UniquePtr&) = delete;

		/**
		 * @brief Copy assignment operator (deleted).
		 */
		UniquePtr& operator=(const UniquePtr&) = delete;

		/**
		 * @brief Move constructor.
		 * @param other UniquePtr to move from.
		 */
		UniquePtr(UniquePtr&& other) noexcept
			: m_ptr(other.m_ptr), m_allocator(Move(other.m_allocator)) {
			other.m_ptr = nullptr;
		}

		/**
		 * @brief Move constructor from a UniquePtr of a different type.
		 * @tparam _Uty Type of the other UniquePtr.
		 * @param other UniquePtr to move from.
		 */
		template<typename _Uty, typename = EnableIfT<IsConvertibleV<_Uty*, _Ty*>>>
		UniquePtr(UniquePtr<_Uty>&& other) noexcept
			: m_ptr(other.Release()), m_allocator(Move(other.GetAllocator())) {}

		/**
		 * @brief Move constructor from a std::unique_ptr.
		 * @param other std::unique_ptr to move from.
		 */
		UniquePtr(std::unique_ptr<_Ty>&& other) noexcept
			: m_ptr(other.release()), m_allocator() {}

		/**
		 * @brief Move assignment operator.
		 * @param other UniquePtr to move from.
		 * @return Reference to this UniquePtr.
		 */
		UniquePtr& operator=(UniquePtr&& other) noexcept {
			if (this != &other) {
				Reset();
				m_ptr = other.m_ptr;
				m_allocator = Move(other.m_allocator);
				other.m_ptr = nullptr;
			}
			return *this;
		}

		/**
		 * @brief Move assignment operator from a std::unique_ptr.
		 * @param other std::unique_ptr to move from.
		 * @return Reference to this UniquePtr.
		 */
		UniquePtr& operator=(std::unique_ptr<_Ty>&& other) noexcept {
			Reset(other.release());
			return *this;
		}

		/**
		 * @brief Destructor. Deletes the managed object.
		 */
		~UniquePtr() {
			if (m_ptr) {
				AllocatorTraits::Destroy(m_allocator, m_ptr);
				AllocatorTraits::Deallocate(m_allocator, m_ptr, 1);
			}
		}

		/**
		 * @brief Returns a pointer to the managed object.
		 * @return Pointer to the managed object.
		 */
		PointerType Get() const { return m_ptr; }

		/**
		 * @brief Releases ownership of the managed object.
		 * @return Pointer to the managed object.
		 */
		PointerType Release() {
			PointerType ptr = m_ptr;
			m_ptr = nullptr;
			return ptr;
		}

		/**
		 * @brief Replaces the managed object.
		 * @param ptr Pointer to the object to manage.
		 */
		void Reset(PointerType ptr = nullptr) {
			if (m_ptr != ptr) {
				if (m_ptr) {
					AllocatorTraits::Destroy(m_allocator, m_ptr);
					AllocatorTraits::Deallocate(m_allocator, m_ptr, 1);
				}
				m_ptr = ptr;
			}
		}

		/**
		 * @brief Dereference operator.
		 * @return Reference to the managed object.
		 */
		ReferenceType operator*() const { return *m_ptr; }

		/**
		 * @brief Member access operator.
		 * @return Pointer to the managed object.
		 */
		PointerType operator->() const { return m_ptr; }

		/**
		 * @brief Boolean conversion operator.
		 * @return true if the UniquePtr owns an object, false otherwise.
		 */
		explicit operator bool() const { return m_ptr != nullptr; }

		/**
		 * @brief Get the allocator associated with this UniquePtr.
		 * @return The allocator.
		 */
		AllocatorType GetAllocator() const { return m_allocator; }

	private:
		PointerType m_ptr;
		AllocatorType m_allocator;
	};

	/**
	 * @brief Create a UniquePtr from a value.
	 * @tparam _Ty The type of the value to manage.
	 * @param args The arguments to pass to the constructor of the managed object.
	 * @return A UniquePtr to the newly created object.
	 */
	template<typename _Ty, typename... Args> UniquePtr<_Ty> MakeUnique(Args&&... args) { return UniquePtr<_Ty>(new _Ty(Forward<Args>(args)...)); }

	/**
	 * @brief Perform a static pointer cast on the managed object.
	 * @tparam To The type to cast to.
	 * @tparam From The type to cast from.
	 * @param from The UniquePtr to cast from.
	 * @return A UniquePtr to the casted object.
	 */
	template<typename To, typename From> UniquePtr<To> StaticPointerCast(UniquePtr<From>&& from) {
		To* casted = static_cast<To*>(from.Get());
		from.Release();
		return UniquePtr<To>(casted);
	}
	
	/**
	 * @brief Perform a dynamic pointer cast on the managed object.
	 * @tparam To The type to cast to.
	 * @tparam From The type to cast from.
	 * @param from The UniquePtr to cast from.
	 * @return A UniquePtr to the casted object, or nullptr if the cast is not possible.
	 */
	template<typename To, typename From> UniquePtr<To> DynamicPointerCast(UniquePtr<From>&& from) {
		To* casted = dynamic_cast<To*>(from.Get());
		if (casted) {
			from.Release();
			return UniquePtr<To>(casted);
		}
		return UniquePtr<To>(nullptr);
	}

	/**
	 * @brief Perform a reinterpret pointer cast on the managed object.
	 * @tparam To The type to cast to.
	 * @tparam From The type to cast from.
	 * @param from The UniquePtr to cast from.
	 * @return A UniquePtr to the casted object.
	 */
	template<typename To, typename From> UniquePtr<To> ReinterpretPointerCast(UniquePtr<From>&& from) {
		To* casted = reinterpret_cast<To*>(from.Get());
		from.Release();
		return UniquePtr<To>(casted);
	}

} // namespace Spark

#endif
