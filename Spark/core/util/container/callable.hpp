#ifndef SPARK_CALLABLE_HPP
#define SPARK_CALLABLE_HPP

#include <core/util/classic/util.hpp>
#include <core/util/log.hpp>
#include <core/util/memory/unique_ptr.hpp>
#include <core/util/classic/traits.hpp>

namespace Spark {

	template<typename Signature> class Callable;

	template<typename ReturnType, typename... Args> class Callable<ReturnType(Args...)> {
	  public:

		Callable() noexcept = default;

		Callable(nullptr_t) noexcept
			: Callable() {}

		template<typename _Fty, typename = EnableIfT<!IsSameV<DecayT<_Fty>, Callable>>> Callable(_Fty&& f)
			: m_function(MakeUnique<CallableImpl<DecayT<_Fty>>>(Forward<_Fty>(f))) {}

		Callable(const Callable& other)
			: m_function(other.m_function ? other.m_function->Clone() : nullptr) {}

		Callable(Callable&& other) noexcept = default;

		Callable& operator=(const Callable& other) {
			Callable(other).Swap(*this);
			return *this;
		}

		Callable& operator=(Callable&& other) noexcept = default;

		Callable& operator=(nullptr_t) noexcept {
			m_function.Reset();
			return *this;
		}

		ReturnType operator()(Args... args) const {
			if (!m_function) {
				LOG_FATAL("Bad function call!");
			}
			return m_function->Invoke(Forward<Args>(args)...);
		}

		explicit operator bool() const noexcept { return static_cast<bool>(m_function); }

		void Swap(Callable& other) noexcept {
			_SPARK Swap(m_function, other.m_function);
		}

	  private:
		class CallableBase {
		  public:
			virtual ~CallableBase()                               = default;
			virtual ReturnType              Invoke(Args...) const = 0;
			virtual UniquePtr<CallableBase> Clone() const         = 0;
		};

		template<typename _Fty> class CallableImpl : public CallableBase {
		  public:
			template<typename Func> explicit CallableImpl(Func&& f)
				: m_f(Forward<Func>(f)) {}

			ReturnType              Invoke(Args... args) const override { return m_f(Forward<Args>(args)...); }
			UniquePtr<CallableBase> Clone() const override { return MakeUnique<CallableImpl>(m_f); }

		  private:
			_Fty m_f;
		};

		UniquePtr<CallableBase> m_function;
	};

	// Deduction guide
	template<typename _Fty> Callable(_Fty) -> Callable<InvokeResultT<_Fty>>;

	// Swap function
	template<typename Signature> void Swap(Callable<Signature>& lhs, Callable<Signature>& rhs) noexcept { lhs.Swap(rhs); }
} // namespace Spark

#endif // SPARK_CALLABLE_HPP