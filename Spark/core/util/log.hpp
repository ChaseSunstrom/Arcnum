#ifndef SPARK_LOG_HPP
#define SPARK_LOG_HPP

#include <iostream>
#include <cassert>
#include "defines.hpp"
#include "time.hpp"
#include "types.hpp"

// Magic windows color codes
#define TEXT_COLOR_GREEN  2
#define TEXT_COLOR_RED    4
#define TEXT_COLOR_CYAN   11
#define TEXT_COLOR_YELLOW 14

void set_console_color(u32 color);

#ifdef __TRACE__
#	define LOG_TRACE(...)                                                                                                                                                                             \
		set_console_color(TEXT_COLOR_CYAN);                                                                                                                                                            \
		std::cout << "[ TRACE " << _SPARK GetCurrentTime() << " ] " << __VA_ARGS__ << "\n"
#else
#	define LOG_TRACE(...)
#endif // __TRACE__

#ifdef __INFO__
#	define LOG_INFO(...)                                                                                                                                                                              \
		set_console_color(TEXT_COLOR_GREEN);                                                                                                                                                           \
		std::cout << "[ INFO " << _SPARK GetCurrentTime() << " ] " << __VA_ARGS__ << "\n"
#else
#	define LOG_INFO(...)
#endif // __INFO__

#ifdef __WARN__
#	define LOG_WARN(...)                                                                                                                                                                              \
		set_console_color(TEXT_COLOR_YELLOW);                                                                                                                                                          \
		std::cout << "[ WARNING " << _SPARK GetCurrentTime() << " ] " << __VA_ARGS__ << "\n"
#else
#	define LOG_WARN(...)
#endif // __WARN__

#ifdef __ERROR__
#	define LOG_ERROR(...)                                                                                                                                                                             \
		set_console_color(TEXT_COLOR_RED);                                                                                                                                                             \
		std::cout << "[ ERROR " << _SPARK GetCurrentTime() << " ] " << __VA_ARGS__ << "\n"
#else
#	define LOG_ERROR(...)
#endif // __ERROR__

#ifdef __FATAL__
#	define LOG_FATAL(...)                                                                                                                                                                             \
		set_console_color(TEXT_COLOR_RED);                                                                                                                                                             \
		std::cout << "[ FATAL " << _SPARK GetCurrentTime() << " ] " << __VA_ARGS__ << "\n";                                                                                                          \
		assert_false
#else
#	define LOG_FATAL(...) assert_false
#endif // __FATAL__

#endif