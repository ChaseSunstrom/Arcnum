#include "time.hpp"
#include <chrono>

namespace Spark {
	String GetCurrentTime() {
		auto     now   = std::chrono::system_clock::now();
		auto     now_c = std::chrono::system_clock::to_time_t(now);
		std::tm* time  = std::localtime(&now_c);

		std::ostringstream oss;
		oss << std::put_time(time, "%Y-%m-%d %H:%M:%S");
		return oss.str();
	}
} // namespace Spark