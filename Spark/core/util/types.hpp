#ifndef SPARK_TYPES_HPP
#define SPARK_TYPES_HPP

#include <cstdint>


/**
 * @brief Type aliases for common integer and floating-point types
 */
using i8  = int8_t;   /**< @brief 8-bit signed integer */
using i16 = int16_t;  /**< @brief 16-bit signed integer */
using i32 = int32_t;  /**< @brief 32-bit signed integer */
using i64 = int64_t;  /**< @brief 64-bit signed integer */
using u8  = uint8_t;  /**< @brief 8-bit unsigned integer */
using u16 = uint16_t; /**< @brief 16-bit unsigned integer */
using u32 = uint32_t; /**< @brief 32-bit unsigned integer */
using u64 = uint64_t; /**< @brief 64-bit unsigned integer */
using f32 = float;    /**< @brief 32-bit floating-point */
using f64 = double;   /**< @brief 64-bit floating-point */

/**
 * @brief Type aliases for special types
 */
using ptrdiff   = ptrdiff_t;        /**< @brief Pointer difference type */
using nullptr_t = decltype(nullptr);/**< @brief Null pointer type */
using usize     = size_t;           /**< @brief Unsigned size type */
using isize     = long long;        /**< @brief Signed size type */

#endif