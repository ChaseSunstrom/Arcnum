#ifndef SPARK_TIME_HPP
#define SPARK_TIME_HPP

#include <core/util/container/string.hpp>
#include <chrono>

namespace Spark {

	/**
	 * @brief A templated timer class for measuring elapsed time.
	 * @tparam TimeUnit The time unit to use for measurements (default: std::chrono::milliseconds).
	 */
	template<typename TimeUnit = std::chrono::milliseconds>
	class Timer {
	public:
		/**
		 * @brief Constructs a Timer and starts it immediately.
		 */
		Timer() : m_start(std::chrono::high_resolution_clock::now()) {}

		/**
		 * @brief Resets the timer to the current time.
		 */
		void Reset() {
			m_start = std::chrono::high_resolution_clock::now();
		}

		/**
		 * @brief Calculates the elapsed time since the timer was started or last reset.
		 * @return The elapsed time in the specified TimeUnit.
		 */
		typename TimeUnit::rep Elapsed() const {
			auto now = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<TimeUnit>(now - m_start);
			return duration.count();
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_start; /**< The start time of the timer. */
	};

	/**
	 * @brief Gets the current time as a formatted string.
	 * @return A String representing the current time.
	 */
	String GetCurrentTime();

} // namespace Spark

#endif