#ifndef SPARK_VARIANT_HPP
#define SPARK_VARIANT_HPP

#include <core/util/classic/util.hpp>
#include <core/util/log.hpp>
#include <type_traits>

namespace Spark {

	// Metafunction to get the index of a type in a list of types
	template<typename _Ty, typename... Ts> struct IndexOfType;

	template<typename _Ty, typename First, typename... Rest> struct IndexOfType<_Ty, First, Rest...> {
		static constexpr size_t value = std::is_same_v<_Ty, First> ? 0 : 1 + IndexOfType<_Ty, Rest...>::value;
	};

	template<typename _Ty> struct IndexOfType<_Ty> {
		static_assert(!std::is_same_v<_Ty, _Ty>, "Type not found in variant!");
	};

	// Metafunction to get the type at a specific index
	template<size_t Index, typename... Types> struct TypeAtIndex;

	template<typename First, typename... Rest> struct TypeAtIndex<0, First, Rest...> {
		using Type = First;
	};

	template<size_t Index, typename First, typename... Rest> struct TypeAtIndex<Index, First, Rest...> {
		using Type = typename TypeAtIndex<Index - 1, Rest...>::Type;
	};

	// Recursive union to store any of the types
	template<typename... Ts> union VariantUnion;

	template<typename First, typename... Rest> union VariantUnion<First, Rest...> {
		First                 first;
		VariantUnion<Rest...> rest;

		VariantUnion()  = default;
		~VariantUnion() = default;

		template<typename _Ty> _Ty* GetPointer() {
			if constexpr (std::is_same_v<_Ty, First>) {
				return &first;
			} else {
				return rest.template GetPointer<_Ty>();
			}
		}

		template<typename _Ty> const _Ty* GetPointer() const {
			if constexpr (std::is_same_v<_Ty, First>) {
				return &first;
			} else {
				return rest.template GetPointer<_Ty>();
			}
		}

		void DestroyAtIndex(size_t index) {
			if (index == 0) {
				first.~First();
			} else {
				rest.DestroyAtIndex(index - 1);
			}
		}

		void CopyAtIndex(VariantUnion& dest, const VariantUnion& src, size_t index) const {
			if (index == 0) {
				new (&dest.first) First(src.first);
			} else {
				rest.CopyAtIndex(dest.rest, src.rest, index - 1);
			}
		}

		void MoveAtIndex(VariantUnion& dest, VariantUnion&& src, size_t index) {
			if (index == 0) {
				new (&dest.first) First(Move(src.first));
			} else {
				rest.MoveAtIndex(dest.rest, Move(src.rest), index - 1);
			}
		}
	};

	template<> union VariantUnion<> {
		VariantUnion()  = default;
		~VariantUnion() = default;

		template<typename _Ty> _Ty* GetPointer() { return nullptr; }

		template<typename _Ty> const _Ty* GetPointer() const { return nullptr; }

		void DestroyAtIndex(size_t) {
			// Base case
		}

		void CopyAtIndex(VariantUnion& dest, const VariantUnion& src, size_t) const {
			// Base case
		}

		void MoveAtIndex(VariantUnion& dest, VariantUnion&& src, size_t) {
			// Base case
		}
	};

	template<typename... Types> class Variant {
	  public:
		Variant()
			: m_type_index(s_invalid_index) {}

		template<typename _Ty, typename = std::enable_if_t<(std::is_same_v<std::decay_t<_Ty>, Types> || ...)>> Variant(_Ty&& val)
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

		template<typename _Ty> bool Is() const { return m_type_index == IndexOfType<_Ty, Types...>::value; }

		template<typename _Ty> _Ty& Get() {
			if (!Is<_Ty>()) {
				LOG_FATAL();
			}
			return *GetPointer<_Ty>();
		}

		template<typename _Ty> const _Ty& Get() const {
			if (!Is<_Ty>()) {
				throw std::bad_variant_access();
			}
			return *GetPointer<_Ty>();
		}

		size_t Index() const { return m_type_index; }

	  private:
		template<typename _Ty> _Ty* GetPointer() { return m_storage.template GetPointer<_Ty>(); }

		template<typename _Ty> const _Ty* GetPointer() const { return m_storage.template GetPointer<_Ty>(); }

		void Destroy() {
			if (m_type_index != s_invalid_index) {
				m_storage.DestroyAtIndex(m_type_index);
				m_type_index = s_invalid_index;
			}
		}

		template<typename _Ty> void Construct(_Ty&& val) {
			using _Uty = std::decay_t<_Ty>;
			new (GetPointer<_Uty>()) _Uty(Forward<_Ty>(val));
			m_type_index = IndexOfType<_Uty, Types...>::value;
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
		static constexpr size_t s_invalid_index = static_cast<size_t>(-1);

		size_t  m_type_index;
		Storage m_storage;
	};

} // namespace Spark

#endif // SPARK_VARIANT_HPP
