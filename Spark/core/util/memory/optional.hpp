#ifndef SPARK_OPTIONAL_HPP
#define SPARK_OPTIONAL_HPP

#include <core/util/assert.hpp>
#include <core/util/classic/traits.hpp>
#include <core/util/classic/util.hpp>
#include <core/util/types.hpp>
#include "allocator.hpp"

namespace Spark {
	/**
	 * @brief A custom optional pointer implementation with allocator support.
	 * @tparam _Ty The type of the optional value.
	 * @tparam Allocator The allocator type used for memory management.
	 */
	template<typename _Ty, typename Allocator = _SPARK Allocator<_Ty>> class OptionalPtr {
	  public:
		/** @brief Alias for the allocator type. */
		using AllocatorType   = Allocator;
		/** @brief Alias for the allocator traits. */
		using AllocatorTraits = AllocatorTraits<AllocatorType>;

		/** @brief Alias for the value type. */
		using ValueType       = typename AllocatorTraits::ValueType;
		/** @brief Alias for the pointer type. */
		using Pointer         = typename AllocatorTraits::Pointer;
		/** @brief Alias for the reference type. */
		using Reference       = typename AllocatorTraits::Reference;
		/** @brief Alias for the const reference type. */
		using ConstReference  = typename AllocatorTraits::ConstReference;
		/** @brief Alias for the const pointer type. */
		using ConstPointer    = typename AllocatorTraits::ConstPointer;
		/** @brief Alias for the size type. */
		using SizeType        = typename AllocatorTraits::SizeType;
		/** @brief Alias for the difference type. */
		using DifferenceType  = typename AllocatorTraits::DifferenceType;

		/**
		 * @brief Default constructor.
		 */
		OptionalPtr()
			: m_has_value(false)
			, m_storage() {}

		/**
		 * @brief Constructor from a const lvalue reference.
		 * @param value The value to store.
		 */
		OptionalPtr(const _Ty& value)
			: m_has_value(true) {
			AllocatorTraits::Construct(m_allocator, &m_storage, value);
		}

		/**
		 * @brief Constructor from an rvalue reference.
		 * @param value The value to store.
		 */
		OptionalPtr(_Ty&& value)
			: m_has_value(true) {
			AllocatorTraits::Construct(m_allocator, &m_storage, Move(value));
		}

		/**
		 * @brief Copy constructor.
		 * @param other The OptionalPtr to copy from.
		 */
		OptionalPtr(const OptionalPtr& other)
			: m_has_value(other.m_has_value) {
			if (m_has_value) {
				AllocatorTraits::Construct(m_allocator, &m_storage, other.Value());
			}
		}

		/**
		 * @brief Move constructor.
		 * @param other The OptionalPtr to move from.
		 */
		OptionalPtr(OptionalPtr&& other) noexcept
			: m_has_value(other.m_has_value) {
			if (m_has_value) {
				AllocatorTraits::Construct(m_allocator, &m_storage, Move(other.Value()));
				other.Reset();
			}
		}

		/**
		 * @brief Destructor.
		 */
		~OptionalPtr() { Reset(); }

		/**
		 * @brief Copy assignment operator.
		 * @param other The OptionalPtr to copy from.
		 * @return Reference to this OptionalPtr.
		 */
		OptionalPtr& operator=(const OptionalPtr& other) {
			if (this != &other) {
				Reset();
				m_has_value = other.m_has_value;
				if (m_has_value) {
					AllocatorTraits::Construct(m_allocator, &m_storage, other.Value());
				}
			}
			return *this;
		}

		/**
		 * @brief Move assignment operator.
		 * @param other The OptionalPtr to move from.
		 * @return Reference to this OptionalPtr.
		 */
		OptionalPtr& operator=(OptionalPtr&& other) noexcept {
			if (this != &other) {
				Reset();
				m_has_value = other.m_has_value;
				if (m_has_value) {
					AllocatorTraits::Construct(m_allocator, &m_storage, Move(other.Value()));
					other.Reset();
				}
			}
			return *this;
		}

		/**
		 * @brief Check if the OptionalPtr contains a value.
		 * @return True if the OptionalPtr contains a value, false otherwise.
		 */
		bool HasValue() const { return m_has_value; }

		/**
		 * @brief Get the stored value (lvalue).
		 * @return Reference to the stored value.
		 */
		_Ty& Value() & {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return *reinterpret_cast<_Ty*>(&m_storage);
		}

		/**
		 * @brief Get the stored value (const lvalue).
		 * @return Const reference to the stored value.
		 */
		const _Ty& Value() const& {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return *reinterpret_cast<const _Ty*>(&m_storage);
		}

		/**
		 * @brief Get the stored value (rvalue).
		 * @return Rvalue reference to the stored value.
		 */
		_Ty&& Value() && {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return Move(*reinterpret_cast<_Ty*>(&m_storage));
		}

		/**
		 * @brief Get the stored value (const rvalue).
		 * @return Const rvalue reference to the stored value.
		 */
		const _Ty&& Value() const&& {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return Move(*reinterpret_cast<const _Ty*>(&m_storage));
		}

		/**
		 * @brief Get the value or a default (const lvalue).
		 * @param default_value The default value to return if no value is stored.
		 * @return The stored value if present, otherwise the default value.
		 */
		template<typename U> _Ty ValueOr(U&& default_value) const& { return m_has_value ? Value() : static_cast<_Ty>(Forward<U>(default_value)); }

		/**
		 * @brief Get the value or a default (rvalue).
		 * @param default_value The default value to return if no value is stored.
		 * @return The stored value if present, otherwise the default value.
		 */
		template<typename U> _Ty ValueOr(U&& default_value) && { return m_has_value ? Move(Value()) : static_cast<_Ty>(Forward<U>(default_value)); }

		/**
		 * @brief Reset the OptionalPtr, destroying any stored value.
		 */
		void Reset() {
			if (m_has_value) {
				AllocatorTraits::Destroy(m_allocator, &m_storage);
				m_has_value = false;
			}
		}

		/**
		 * @brief Emplace a new value in-place.
		 * @param args Arguments to forward to the constructor of _Ty.
		 * @return Reference to the newly constructed value.
		 */
		template<typename... Args> _Ty& Emplace(Args&&... args) {
			Reset();
			AllocatorTraits::Construct(m_allocator, &m_storage, Forward<Args>(args)...);
			m_has_value = true;
			return Value();
		}

		/**
		 * @brief Boolean conversion operator.
		 * @return True if the OptionalPtr contains a value, false otherwise.
		 */
		explicit operator bool() const { return m_has_value; }

		/**
		 * @brief Arrow operator.
		 * @return Pointer to the stored value.
		 */
		_Ty* operator->() {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return reinterpret_cast<_Ty*>(&m_storage);
		}

		/**
		 * @brief Const arrow operator.
		 * @return Const pointer to the stored value.
		 */
		const _Ty* operator->() const {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return reinterpret_cast<const _Ty*>(&m_storage);
		}

		/**
		 * @brief Dereference operator (lvalue).
		 * @return Reference to the stored value.
		 */
		_Ty& operator*() & { return Value(); }

		/**
		 * @brief Dereference operator (const lvalue).
		 * @return Const reference to the stored value.
		 */
		const _Ty& operator*() const& { return Value(); }

		/**
		 * @brief Dereference operator (rvalue).
		 * @return Rvalue reference to the stored value.
		 */
		_Ty&& operator*() && { return Move(Value()); }

		/**
		 * @brief Dereference operator (const rvalue).
		 * @return Const rvalue reference to the stored value.
		 */
		const _Ty&& operator*() const&& { return Move(Value()); }

	  private:
		/** @brief Flag indicating whether a value is stored. */
		bool m_has_value;
		/** @brief Storage for the optional value. */
		alignas(_Ty) u8 m_storage[sizeof(_Ty)];
		/** @brief The allocator instance. */
		AllocatorType m_allocator;
	};

	/**
	 * @brief Create an OptionalPtr from a value.
	 * @tparam _Ty The type of the value to store.
	 * @param value The value to store in the OptionalPtr.
	 * @return An OptionalPtr containing the value.
	 */
	template<typename _Ty> OptionalPtr<DecayT<_Ty>> MakeOptionalPtr(_Ty&& value) { return OptionalPtr<DecayT<_Ty>>(Forward<_Ty>(value)); }

	/**
	 * @brief Create an OptionalPtr from a value.
	 * @tparam _Ty The type of the value to store.
	 * @param args The arguments to forward to the constructor of _Ty.
	 * @return An OptionalPtr containing the value.
	 */
	template<typename _Ty, typename... Args> OptionalPtr<_Ty> MakeOptionalPtr(Args&&... args) { return OptionalPtr<_Ty>(_Ty(Forward<Args>(args)...)); }
} // namespace Spark

#endif // SPARK_OPTIONAL_HPP