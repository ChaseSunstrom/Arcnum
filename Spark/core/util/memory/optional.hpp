#ifndef SPARK_OPTIONAL_HPP
#define SPARK_OPTIONAL_HPP

#include <core/util/types.hpp>
#include <core/util/classic/util.hpp>
#include <core/util/classic/traits.hpp>

namespace Spark {
	template<typename T>
	class OptionalPtr {
	public:
		OptionalPtr() : m_has_value(false), m_storage() {}

		OptionalPtr(const T& value) : m_has_value(true) {
			new (&m_storage) T(value);
		}

		OptionalPtr(T&& value) : m_has_value(true) {
			new (&m_storage) T(Move(value));
		}

		OptionalPtr(const OptionalPtr& other) : m_has_value(other.m_has_value) {
			if (m_has_value) {
				new (&m_storage) T(other.Value());
			}
		}

		OptionalPtr(OptionalPtr&& other) noexcept : m_has_value(other.m_has_value) {
			if (m_has_value) {
				new (&m_storage) T(Move(other.Value()));
				other.Reset();
			}
		}

		~OptionalPtr() {
			Reset();
		}

		OptionalPtr& operator=(const OptionalPtr& other) {
			if (this != &other) {
				Reset();
				m_has_value = other.m_has_value;
				if (m_has_value) {
					new (&m_storage) T(other.Value());
				}
			}
			return *this;
		}

		OptionalPtr& operator=(OptionalPtr&& other) noexcept {
			if (this != &other) {
				Reset();
				m_has_value = other.m_has_value;
				if (m_has_value) {
					new (&m_storage) T(Move(other.Value()));
					other.Reset();
				}
			}
			return *this;
		}

		bool HasValue() const { return m_has_value; }

		T& Value() & {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return *reinterpret_cast<T*>(&m_storage);
		}

		const T& Value() const& {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return *reinterpret_cast<const T*>(&m_storage);
		}

		T&& Value() && {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return Move(*reinterpret_cast<T*>(&m_storage));
		}

		const T&& Value() const&& {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return Move(*reinterpret_cast<const T*>(&m_storage));
		}

		template<typename U>
		T ValueOr(U&& default_value) const& {
			return m_has_value ? Value() : static_cast<T>(Forward<U>(default_value));
		}

		template<typename U>
		T ValueOr(U&& default_value) && {
			return m_has_value ? Move(Value()) : static_cast<T>(Forward<U>(default_value));
		}

		void Reset() {
			if (m_has_value) {
				reinterpret_cast<T*>(&m_storage)->~T();
				m_has_value = false;
			}
		}

		template<typename... Args>
		T& Emplace(Args&&... args) {
			Reset();
			new (&m_storage) T(Forward<Args>(args)...);
			m_has_value = true;
			return Value();
		}

		explicit operator bool() const { return m_has_value; }

		T* operator->() {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return reinterpret_cast<T*>(&m_storage);
		}

		const T* operator->() const {
			SPARK_ASSERT(m_has_value, "Accessing value of empty Optional");
			return reinterpret_cast<const T*>(&m_storage);
		}

		T& operator*() & {
			return Value();
		}

		const T& operator*() const& {
			return Value();
		}

		T&& operator*() && {
			return Move(Value());
		}

		const T&& operator*() const&& {
			return Move(Value());
		}

	private:
		bool m_has_value;
		alignas(T) u8 m_storage[sizeof(T)];
	};

	template<typename T>
	OptionalPtr<DecayT<T>> MakeOptionalPtr(T&& value) {
		return OptionalPtr<DecayT<T>>(Forward<T>(value));
	}

	template<typename T, typename... Args>
	OptionalPtr<T> MakeOptionalPtr(Args&&... args) {
		return OptionalPtr<T>(T(Forward<Args>(args)...));
	}
} // namespace Spark



#endif // SPARK_OPTIONAL_HPP