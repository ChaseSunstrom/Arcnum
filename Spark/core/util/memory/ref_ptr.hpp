#ifndef SPARK_REF_PTR_HPP
#define SPARK_REF_PTR_HPP

#include <core/pch.hpp>
#include "shared_ptr.hpp"
#include "unique_ptr.hpp"
#include "weak_ptr.hpp"

namespace Spark {
	/**
	 * @brief A non-owning reference to a pointer.
	 * 
	 * This class is useful for passing around pointers without taking ownership.
	 * It's particularly helpful when using a unique_ptr for memory management
	 * but also needing to store the pointer elsewhere without using a raw pointer
	 * or a shared_ptr.
	 * 
	 * @tparam _Ty The type of the object being pointed to.
	 */
	template<typename _Ty>
	class RefPtr {
	public:
		/**
		 * @brief Construct a RefPtr from a const pointer.
		 * @param ptr The pointer to reference.
		 */
		RefPtr(const _Ty* ptr) : m_ptr(ptr) {}

		/**
		 * @brief Construct a RefPtr from a non-const pointer.
		 * @param ptr The pointer to reference.
		 */
		RefPtr(_Ty* ptr) : m_ptr(ptr) {}

		/**
		 * @brief Construct a RefPtr from a const reference.
		 * @param ref The reference to point to.
		 */
		RefPtr(const _Ty& ref) : m_ptr(&ref) {}

		/**
		 * @brief Construct a RefPtr from a non-const reference.
		 * @param ref The reference to point to.
		 */
		RefPtr(_Ty& ref) : m_ptr(&ref) {}

		/**
		 * @brief Construct a null RefPtr.
		 */
		RefPtr(nullptr_t) : m_ptr(nullptr) {}

		/**
		 * @brief Default constructor. Creates a null RefPtr.
		 */
		RefPtr() : m_ptr(nullptr) {}

		/**
		 * @brief Copy constructor.
		 * @param other The RefPtr to copy from.
		 */
		RefPtr(const RefPtr& other) : m_ptr(other.m_ptr) {}

		/**
		 * @brief Move constructor.
		 * @param other The RefPtr to move from.
		 */
		RefPtr(RefPtr&& other) noexcept : m_ptr(other.m_ptr) {}

		/**
		 * @brief Construct from a std::unique_ptr.
		 * @param ptr The unique_ptr to reference.
		 */
		RefPtr(const std::unique_ptr<_Ty>& ptr) : m_ptr(ptr.get()) {}

		/**
		 * @brief Construct from a std::shared_ptr.
		 * @param ptr The shared_ptr to reference.
		 */
		RefPtr(const std::shared_ptr<_Ty>& ptr) : m_ptr(ptr.get()) {}

		/**
		 * @brief Construct from a std::weak_ptr.
		 * @param ptr The weak_ptr to reference.
		 */
		RefPtr(const std::weak_ptr<_Ty>& ptr) : m_ptr(ptr.lock().get()) {}

		/**
		 * @brief Construct from a UniquePtr.
		 * @param ptr The UniquePtr to reference.
		 */
		RefPtr(const UniquePtr<_Ty>& ptr) : m_ptr(ptr.Get()) {}

		/**
		 * @brief Construct from a SharedPtr.
		 * @param ptr The SharedPtr to reference.
		 */
		RefPtr(const SharedPtr<_Ty>& ptr) : m_ptr(ptr.Get()) {}

		/**
		 * @brief Construct from a WeakPtr.
		 * @param ptr The WeakPtr to reference.
		 */
		RefPtr(const WeakPtr<_Ty>& ptr) : m_ptr(ptr.Get()) {}

		/**
		 * @brief Implicit conversion to the underlying reference.
		 * @return Reference to the pointed object.
		 */
		operator _Ty&() const { return *m_ptr; }

		/**
		 * @brief Implicit conversion to the underlying pointer.
		 * @return Pointer to the object.
		 */
		operator _Ty*() const { return m_ptr; }

		/**
		 * @brief Copy assignment operator.
		 * @param other The RefPtr to copy from.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(const RefPtr& other) {
			m_ptr = other.m_ptr;
			return *this;
		}

		/**
		 * @brief Move assignment operator.
		 * @param other The RefPtr to move from.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(RefPtr&& other) noexcept {
			m_ptr = other.m_ptr;
			return *this;
		}

		/**
		 * @brief Assignment from a std::unique_ptr.
		 * @param ptr The unique_ptr to assign from.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(const std::unique_ptr<_Ty>& ptr) {
			m_ptr = ptr.get();
			return *this;
		}

		/**
		 * @brief Assignment from a std::shared_ptr.
		 * @param ptr The shared_ptr to assign from.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(const std::shared_ptr<_Ty>& ptr) {
			m_ptr = ptr.get();
			return *this;
		}

		/**
		 * @brief Assignment from a std::weak_ptr.
		 * @param ptr The weak_ptr to assign from.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(const std::weak_ptr<_Ty>& ptr) {
			m_ptr = ptr.lock().get();
			return *this;
		}

		/**
		 * @brief Assignment from a UniquePtr.
		 * @param ptr The UniquePtr to assign from.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(const UniquePtr<_Ty>& ptr) {
			m_ptr = ptr.Get();
			return *this;
		}

		/**
		 * @brief Assignment from a SharedPtr.
		 * @param ptr The SharedPtr to assign from.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(const SharedPtr<_Ty>& ptr) {
			m_ptr = ptr.Get();
			return *this;
		}

		/**
		 * @brief Assignment from a WeakPtr.
		 * @param ptr The WeakPtr to assign from.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(const WeakPtr<_Ty>& ptr) {
			m_ptr = ptr.Lock().Get();
			return *this;
		}

		/**
		 * @brief Assignment from a const pointer.
		 * @param ptr The pointer to assign.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(const _Ty* ptr) {
			m_ptr = ptr;
			return *this;
		}

		/**
		 * @brief Assignment from a non-const pointer.
		 * @param ptr The pointer to assign.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(_Ty* ptr) {
			m_ptr = ptr;
			return *this;
		}

		/**
		 * @brief Assignment of nullptr.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(nullptr_t) {
			m_ptr = nullptr;
			return *this;
		}

		/**
		 * @brief Assignment from a const reference.
		 * @param ptr The reference to assign.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(const _Ty& ptr) {
			m_ptr = &ptr;
			return *this;
		}

		/**
		 * @brief Assignment from an rvalue reference.
		 * @param ptr The rvalue reference to assign.
		 * @return Reference to this RefPtr.
		 */
		RefPtr& operator=(const _Ty&& ptr) {
			m_ptr = &ptr;
			return *this;
		}

		/**
		 * @brief Check if the RefPtr is null.
		 * @return true if the RefPtr is null, false otherwise.
		 */
		bool operator!() const { return m_ptr == nullptr; }

		/**
		 * @brief Check if the RefPtr is not null.
		 * @return true if the RefPtr is not null, false otherwise.
		 */
		explicit operator bool() const { return m_ptr != nullptr; }

		/**
		 * @brief Arrow operator.
		 * @return Pointer to the referenced object.
		 */
		_Ty* operator->() const { return m_ptr; }

		/**
		 * @brief Dereference operator.
		 * @return Reference to the pointed object.
		 */
		_Ty& operator*() const { return *m_ptr; }

		/**
		 * @brief Equality comparison with another RefPtr.
		 * @param other The RefPtr to compare with.
		 * @return true if the pointers are equal, false otherwise.
		 */
		bool operator==(const RefPtr<_Ty>& other) const { return m_ptr == other.m_ptr; }

		/**
		 * @brief Inequality comparison with another RefPtr.
		 * @param other The RefPtr to compare with.
		 * @return true if the pointers are not equal, false otherwise.
		 */
		bool operator!=(const RefPtr<_Ty>& other) const { return m_ptr != other.m_ptr; }

		/**
		 * @brief Equality comparison with a raw pointer.
		 * @param other The pointer to compare with.
		 * @return true if the pointers are equal, false otherwise.
		 */
		bool operator==(const _Ty* other) const { return m_ptr == other; }

		/**
		 * @brief Inequality comparison with a raw pointer.
		 * @param other The pointer to compare with.
		 * @return true if the pointers are not equal, false otherwise.
		 */
		bool operator!=(const _Ty* other) const { return m_ptr != other; }

		/**
		 * @brief Equality comparison with nullptr.
		 * @return true if the RefPtr is null, false otherwise.
		 */
		bool operator==(nullptr_t) const { return m_ptr == nullptr; }

		/**
		 * @brief Inequality comparison with nullptr.
		 * @return true if the RefPtr is not null, false otherwise.
		 */
		bool operator!=(nullptr_t) const { return m_ptr != nullptr; }

		/**
		 * @brief Get the underlying pointer.
		 * @return The raw pointer.
		 */
		_Ty* Get() const { return m_ptr; }

		/**
		 * @brief Get the underlying pointer as const.
		 * @return The const raw pointer.
		 */
		const _Ty* GetConst() const { return m_ptr; }

	private:
		_Ty* m_ptr; /**< The underlying pointer. */
	};

	/**
	 * @brief Deduction guide for RefPtr.
	 * @tparam Args The types of the arguments.
	 * @param args The arguments to construct the RefPtr.
	 * @return A RefPtr constructed from the arguments.
	 */
	template<typename... Args> RefPtr(Args... args) -> RefPtr<std::common_type_t<Args...>>;

	/**
	 * @brief Make a RefPtr from a variadic list of arguments.
	 * @tparam _Ty The type of the RefPtr.
	 * @param args The arguments to construct the RefPtr.
	 * @return A RefPtr constructed from the arguments.
	 */
	template<typename _Ty, typename... Args> RefPtr<_Ty> MakeRefPtr(Args... args) { return RefPtr<_Ty>(args...); }

	/**
	 * @brief Make a RefPtr from a raw pointer.
	 * @tparam _Ty The type of the RefPtr.
	 * @param ptr The raw pointer to reference.
	 * @return A RefPtr constructed from the pointer.
	 */
	template<typename _Ty> RefPtr<_Ty> MakeRefPtr(const _Ty* ptr) { return RefPtr<_Ty>(ptr); }

	/**
	 * @brief Make a RefPtr from a const reference.
	 * @tparam _Ty The type of the RefPtr.
	 * @param ref The const reference to point to.
	 * @return A RefPtr constructed from the reference.
	 */
	template<typename _Ty> RefPtr<_Ty> MakeRefPtr(const _Ty& ref) { return RefPtr<_Ty>(&ref); }

	/**
	 * @brief Cast a RefPtr to a different type.
	 * @tparam To The type to cast to.
	 * @tparam From The type to cast from.
	 * @param from The RefPtr to cast.
	 * @return A RefPtr of the new type.
	 */
	template<typename To, typename From> RefPtr<To> RefCast(const RefPtr<From>& from) { return RefPtr<To>(static_cast<To*>(const_cast<From*>(from.Get()))); }

	/**
	 * @brief Cast a RefPtr to a different type.
	 * @tparam To The type to cast to.
	 * @tparam From The type to cast from.
	 * @param from The RefPtr to cast.
	 * @return A RefPtr of the new type.
	 */
	template<typename To, typename From> RefPtr<const To> RefCast(const RefPtr<const From>& from) { return RefPtr<const To>(dynamic_cast<const To*>(from.Get())); }
} // namespace Spark

#endif // SPARK_REF_PTR_HPP