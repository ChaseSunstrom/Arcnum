#ifndef SPARK_ASSERT_HPP
#define SPARK_ASSERT_HPP

#include "defines.hpp"
#include "log.hpp"

namespace Spark {
// inline function to handle assertions
inline void Assert(bool condition, const char* message, const char* file, int line) {
	if (!condition) {
		LOG_FATAL("Assertion failed: " << message << " at " << file << ":" << line);
	}
}
}

#define SPARK_ASSERT(condition, message) ::Spark::Assert(condition, message, __FILE__, __LINE__)

#endif