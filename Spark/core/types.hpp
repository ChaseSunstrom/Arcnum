#ifndef CORE_TYPES_HPP
#define CORE_TYPES_HPP

#include <cstdint>

namespace spark
{
#if !defined(u8)
	using u8 = std::uint8_t;
#endif
	
#if !defined(i8)
	using i8 = std::int8_t;
#endif

#if !defined(u16)
	using u16 = std::uint16_t;
#endif

#if !defined(i16)
	using i16 = std::int16_t;
#endif

#if !defined(i32)
	using i32 = std::int32_t;
#endif

#if !defined(u32)
	using u32 = std::uint32_t;
#endif

#if !defined(i64)
	using i64 = std::int64_t;
#endif

#if !defined(u64)
	using u64 = std::uint64_t;
#endif

#if !defined(f32)
	using f32 = float;
#endif

#if !defined(f64)
	using f64 = double;
#endif
}

#endif