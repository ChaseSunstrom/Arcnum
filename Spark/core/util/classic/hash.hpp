#ifndef SPARK_HASH_HPP
#define SPARK_HASH_HPP

#include <core/util/types.hpp>
#include "traits.hpp"

namespace Spark {

	// Put these all into one function lmao
	template<typename _Ty> usize HashValue(const _Ty& value) {
		if constexpr (IsIntegralV<_Ty> || IsEnumV<_Ty>) {
			return static_cast<usize>(value) * 2654435761u;
		} else if constexpr (IsFloatingPointV<_Ty>) {
			usize                hash = 0;
			const unsigned char* p    = reinterpret_cast<const unsigned char*>(&value);
			for (usize i = 0; i < sizeof(_Ty); ++i) {
				hash = hash * 31 + p[i];
			}
			return hash;
		} else if constexpr (IsSameV<_Ty, const char*>) {
			usize                hash = 5381;
			const unsigned char* s    = reinterpret_cast<const unsigned char*>(value);
			while (*s) {
				hash = ((hash << 5) + hash) + *s++;
			}
			return hash;
		} else if constexpr (IsPointerV<_Ty> && !IsSameV<RemovePointerT<_Ty>, char>) {
			return reinterpret_cast<usize>(value) >> 3;
		} else {
			static_assert(sizeof(_Ty) == 0, "HashValue not implemented for this type.");
		}
	}

	template<typename _Ty> struct Hash {
		usize operator()(const _Ty& value) const { return HashValue(value); }
	};

} // namespace Spark

#endif