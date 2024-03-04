#ifndef CORE_TYPES_HPP
#define CORE_TYPES_HPP

#include <cstdint>

namespace spark
{
	using c_str = const char*;

	using byte = std::uint8_t;
	using sbyte = std::int8_t;

	using int32_t = std::int32_t;
	using uint32_t = std::uint32_t;
	using int64_t = std::int64_t;
	using uint64_t = std::uint64_t;

	using float32_t = float;
	using float64_t = double;
}

#endif