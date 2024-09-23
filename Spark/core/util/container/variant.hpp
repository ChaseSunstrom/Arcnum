#ifndef SPARK_VARIANT_HPP
#define SPARK_VARIANT_HPP

#include <core/util/classic/traits.hpp>
#include <core/util/classic/util.hpp>
#include <core/util/types.hpp>
#include <core/util/log.hpp>

namespace Spark {
	
	static constexpr size_t  invalid_index = static_cast<size_t >(-1);

	template<typename _Ty, typename... Ts> struct IndexOfType;

	template<typename _Ty, typename First, typename... Rest> struct IndexOfType<_Ty, First, Rest...> {
		static constexpr size_t  temp  = IndexOfType<_Ty, Rest...>::Value;
		static constexpr size_t  Value = IsSameV<_Ty, First> ? 0 : (temp == invalid_index ? invalid_index : 1 + temp);
	};

	template<typename _Ty> struct IndexOfType<_Ty> {
		static constexpr size_t  Value = invalid_index;
	};

	template<size_t  Index, typename... Types> struct TypeAtIndex;

	template<typename First, typename... Rest> struct TypeAtIndex<0, First, Rest...> {
		using Type = First;
	};

	template<size_t  Index, typename First, typename... Rest> struct TypeAtIndex<Index, First, Rest...> {
		static_assert(Index < sizeof...(Rest) + 1, "Index out of bounds");
		using Type = typename TypeAtIndex<Index - 1, Rest...>::Type;
	};

	template<typename... Ts> union VariantUnion;

	template<typename First, typename... Rest> union VariantUnion<First, Rest...> {
		First                 first;
		VariantUnion<Rest...> rest;

		VariantUnion() {}
		~VariantUnion() {}

		template<typename _Ty> _Ty* GetPointer() {
			if constexpr (IsSameV<_Ty, First>) {
				return &first;
			} else {
				return rest.template GetPointer<_Ty>();
			}
		}

		template<typename _Ty> const _Ty* GetPointer() const {
			if constexpr (IsSameV<_Ty, First>) {
				return &first;
			} else {
				return rest.template GetPointer<_Ty>();
			}
		}

		void DestroyAtIndex(size_t  index) {
			if (index == 0) {
				first.~First();
			} else {
				rest.DestroyAtIndex(index - 1);
			}
		}

		void CopyAtIndex(VariantUnion& dest, const VariantUnion& src, size_t  index) const {
			if (index == 0) {
				new (&dest.first) First(src.first);
			} else {
				rest.CopyAtIndex(dest.rest, src.rest, index - 1);
			}
		}

		void MoveAtIndex(VariantUnion& dest, VariantUnion&& src, size_t  index) {
			if (index == 0) {
				new (&dest.first) First(Move(src.first));
			} else {
				rest.MoveAtIndex(dest.rest, Move(src.rest), index - 1);
			}
		}
	};

	// Base case (no types)
	template<> union VariantUnion<> {
		VariantUnion() {}
		~VariantUnion() {}

		template<typename _Ty> _Ty*       GetPointer() { return nullptr; }
		template<typename _Ty> const _Ty* GetPointer() const { return nullptr; }

		// Destroy does nothing as there are no active members
		void DestroyAtIndex(size_t ) {
			// Base case: nothing to destroy
		}

		// Copy does nothing as there are no active members
		void CopyAtIndex(VariantUnion& dest, const VariantUnion& src, size_t ) const {
			// Base case: nothing to copy
		}

		// Move does nothing as there are no active members
		void MoveAtIndex(VariantUnion& dest, VariantUnion&& src, size_t ) {
			// Base case: nothing to move
		}
	};

	template<typename... Types> class Variant {
	  public:
		Variant()
			: m_type_index(s_invalid_index) {}

		// Templated constructor: enabled only if DecayT<_Ty> is one of Types...
		template<typename _Ty, typename = EnableIfT<(IsSameV<DecayT<_Ty>, Types> || ...) || (IsConstructibleV<DecayT<_Ty>, Types> || ...)>> Variant(_Ty&& val)

			: m_type_index(s_invalid_index) {
			Construct(Forward<_Ty>(val));
		}

		Variant(const Variant& other)
			: m_type_index(s_invalid_index) {
			CopyFrom(other);
		}

		Variant(Variant&& other) noexcept
			: m_type_index(s_invalid_index) {
			MoveFrom(Move(other));
		}

		Variant& operator=(const Variant& other) {
			if (this != &other) {
				Destroy();
				CopyFrom(other);
			}
			return *this;
		}

		Variant& operator=(Variant&& other) noexcept {
			if (this != &other) {
				Destroy();
				MoveFrom(Move(other));
			}
			return *this;
		}

		~Variant() { Destroy(); }

		template<typename _Ty> bool Is() const { return m_type_index == IndexOfType<DecayT<_Ty>, Types...>::Value; }

		template<typename _Ty> _Ty& Get() {
			if (!Is<_Ty>()) {
				LOG_FATAL("Bad variant get!");
			}
			return *GetPointer<_Ty>();
		}

		template<typename _Ty> Variant& operator=(_Ty&& val) {
			using Uty = DecayT<_Ty>;

			// Ensure the value being assigned can be converted to one of the types in the Variant
			static_assert((IsSameV<Uty, Types> || ...), "Type not supported by this Variant!");

			Destroy();
			Construct(Forward<_Ty>(val));
			return *this;
		}

		template<typename _Ty> const _Ty& Get() const {
			if (!Is<_Ty>()) {
				LOG_FATAL("Bad variant access!");
			}
			return *GetPointer<_Ty>();
		}

		size_t  Index() const { return m_type_index; }

	  private:
		template<typename _Ty> _Ty* GetPointer() { return m_storage.template GetPointer<DecayT<_Ty>>(); }

		template<typename _Ty> const _Ty* GetPointer() const { return m_storage.template GetPointer<DecayT<_Ty>>(); }

		void Destroy() {
			if (m_type_index != s_invalid_index) {
				m_storage.DestroyAtIndex(m_type_index);
				m_type_index = s_invalid_index;
			}
		}

		template<typename _Ty> void Construct(_Ty&& val) {
			using Uty = DecayT<_Ty>;
			new (GetPointer<Uty>()) Uty(Forward<_Ty>(val));
			m_type_index = IndexOfType<Uty, Types...>::Value;
		}

		void CopyFrom(const Variant& other) {
			if (other.m_type_index != s_invalid_index) {
				m_storage.CopyAtIndex(m_storage, other.m_storage, other.m_type_index);
				m_type_index = other.m_type_index;
			}
		}

		void MoveFrom(Variant&& other) {
			if (other.m_type_index != s_invalid_index) {
				m_storage.MoveAtIndex(m_storage, Move(other.m_storage), other.m_type_index);
				m_type_index = other.m_type_index;
				other.Destroy();
			}
		}

		using Storage                           = VariantUnion<Types...>;
		static constexpr size_t  s_invalid_index = static_cast<size_t >(-1);

		size_t   m_type_index;
		Storage m_storage;
	};

} // namespace Spark

#endif // SPARK_VARIANT_HPP
