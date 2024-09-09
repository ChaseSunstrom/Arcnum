#ifndef SPARK_CALLABLE_HPP
#define SPARK_CALLABLE_HPP

#include <core/util/log.hpp>
#include <core/util/classic/util.hpp>
#include <core/util/memory/unique_ptr.hpp>

namespace Spark {
	template<typename Signature> class Callable;

	template<typename ReturnType, typename... Args> class Callable<ReturnType(Args...)> {
	public:
		// Default constructor
		Callable() noexcept = default;

		// Null pointer constructor
		Callable(std::nullptr_t) noexcept : Callable() {}

		// Constructor taking any callable object
		template<typename F, typename = std::enable_if_t<!std::is_same_v<std::decay_t<F>, Callable>>> Callable(F&& f)
			: m_function(MakeUnique<CallableImpl<std::decay_t<F>>>(Forward<F>(f))) {}

		// Copy constructor
		Callable(const Callable& other) : m_function(other.m_function ? other.m_function->Clone() : nullptr) {}

		// Move constructor
		Callable(Callable&& other) noexcept = default;

		// Copy assignment operator
		Callable& operator=(const Callable& other) {
			if (this != &other) { m_function = other.m_function ? other.m_function->Clone() : nullptr; }
			return *this;
		}

		// Move assignment operator
		Callable& operator=(Callable&& other) noexcept = default;

		// Null assignment operator
		Callable& operator=(std::nullptr_t) noexcept {
			m_function.reset();
			return *this;
		}

		// Function call operator
		ReturnType operator()(Args... args) const {
			if (!m_function) { LOG_FATAL("Bad function call!"); }
			return m_function->Invoke(Forward<Args>(args)...);
		}

		// Check if the Callable contains a valid function
		explicit operator bool() const noexcept { return static_cast<bool>(m_function); }

		// Swap function
		void Swap(Callable& other) noexcept { m_function.swap(other.m_function); }

	private:
		// Base class for callable objects
		class CallableBase {
		public:
			virtual                               ~CallableBase() = default;
			virtual ReturnType                    Invoke(Args...) const = 0;
			virtual UniquePtr<CallableBase> Clone() const = 0;
		};

		// Derived class to store the actual callable object
		template<typename F> class CallableImpl : public CallableBase {
		public:
			explicit CallableImpl(F&& f) : m_f(Forward<F>(f)) {}

			ReturnType Invoke(Args... args) const override { return m_f(Forward<Args>(args)...); }

			UniquePtr<CallableBase> Clone() const override { return MakeUnique<CallableImpl>(m_f); }

		private:
			F m_f;
		};

		UniquePtr<CallableBase> m_function;
	};

	// Deduction guide
	template<typename F> Callable(F) -> Callable<typename std::invoke_result_t<F>>;

	// Swap function
	template<typename Signature> void Swap(Callable<Signature>& lhs, Callable<Signature>& rhs) noexcept { lhs.Swap(rhs); }
}

#endif //SPARK_CALLABLE_HPP