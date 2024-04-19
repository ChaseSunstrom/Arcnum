#ifndef SPARK_TIMER_HPP
#define SPARK_TIMER_HPP

#include "../spark.hpp"

namespace spark
{
	class timer
	{
	public:
		timer() :
				m_is_running(false) { }

		void start()
		{
			m_start_time = std::chrono::high_resolution_clock::now();
			m_is_running = true;
		}

		void stop()
		{
			m_start_time = std::chrono::high_resolution_clock::now();
			m_is_running = false;
		}

		i64 elapsed_seconds() const
		{
			if (m_is_running)
			{
				auto end_time = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - m_start_time);
				return duration.count();
			}
			return 0;
		}

		i64 elapsed_milliseconds() const
		{
			if (m_is_running)
			{
				auto end_time = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - m_start_time);
				return duration.count();
			}
			return 0;
		}

		i64 elapsed_microseconds() const
		{
			if (m_is_running)
			{
				auto end_time = std::chrono::high_resolution_clock::now();
				auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - m_start_time);
				return duration.count();
			}

			return 0;
		}

	private:
		std::chrono::time_point <std::chrono::high_resolution_clock> m_start_time;

		bool m_is_running;
	};
}

#endif