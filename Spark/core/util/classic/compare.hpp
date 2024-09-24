#ifndef SPARK_COMPARE_HPP
#define SPARK_COMPARE_HPP

namespace Spark {
	/**
	 * @brief Comparison functors for common operations
	 */

	/**
	 * @brief Less-than comparison functor
	 * @tparam T The type of the elements to compare
	 */
	template <typename T>
	struct Less {
		/**
		 * @brief Performs less-than comparison
		 * @param a First element
		 * @param b Second element
		 * @return true if a < b, false otherwise
		 */
		bool operator()(const T& a, const T& b) const { return a < b; }
	};

	/**
	 * @brief Greater-than comparison functor
	 * @tparam T The type of the elements to compare
	 */
	template <typename T>
	struct Greater {
		/**
		 * @brief Performs greater-than comparison
		 * @param a First element
		 * @param b Second element
		 * @return true if a > b, false otherwise
		 */
		bool operator()(const T& a, const T& b) const { return a > b; }
	};

	/**
	 * @brief Equality comparison functor
	 * @tparam T The type of the elements to compare
	 */
	template <typename T>
	struct Equal {
		/**
		 * @brief Performs equality comparison
		 * @param a First element
		 * @param b Second element
		 * @return true if a == b, false otherwise
		 */
		bool operator()(const T& a, const T& b) const { return a == b; }
	};

	/**
	 * @brief Inequality comparison functor
	 * @tparam T The type of the elements to compare
	 */
	template <typename T>
	struct NotEqual {
		/**
		 * @brief Performs inequality comparison
		 * @param a First element
		 * @param b Second element
		 * @return true if a != b, false otherwise
		 */
		bool operator()(const T& a, const T& b) const { return a != b; }
	};
}


#endif