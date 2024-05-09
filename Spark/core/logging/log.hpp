#ifndef SPARK_LOG_HPP
#define SPARK_LOG_HPP

#include "../spark.hpp"

namespace Spark
{

// Magic windows color codes
#define TEXT_COLOR_GREEN 2
#define TEXT_COLOR_RED 4
#define TEXT_COLOR_CYAN 11
#define TEXT_COLOR_YELLOW 14

void set_console_color(u32 color);

#ifdef __SPARK_TRACE__
#define SPARK_TRACE(...)                                                                                               \
    Spark::set_console_color(TEXT_COLOR_CYAN);                                                                         \
    std::cout << "[ TRACE ] " << __VA_ARGS__ << "\n"
#else
#define SPARK_TRACE(...)
#endif // __SPARK_TRACE__

#ifdef __SPARK_INFO__
#define SPARK_INFO(...)                                                                                                \
    Spark::set_console_color(TEXT_COLOR_GREEN);                                                                        \
    std::cout << "[ INFO ] " << __VA_ARGS__ << "\n"
#else
#define SPARK_INFO(...)
#endif // __SPARK_INFO__

#ifdef __SPARK_WARN__
#define SPARK_WARN(...)                                                                                                \
    Spark::set_console_color(TEXT_COLOR_YELLOW);                                                                       \
    std::cout << "[ WARNING ] " << __VA_ARGS__ << "\n"
#else
#define SPARK_WARN(...)
#endif // __SPARK_WARN__

#ifdef __SPARK_ERROR__
#define SPARK_ERROR(...)                                                                                               \
    Spark::set_console_color(TEXT_COLOR_RED);                                                                          \
    std::cout << "[ ERROR ] " << __VA_ARGS__ << "\n"
#else
#define SPARK_ERROR(...)
#endif // __SPARK_ERROR__

#ifdef __SPARK_FATAL__
#define SPARK_FATAL(...)                                                                                               \
    Spark::set_console_color(TEXT_COLOR_RED);                                                                          \
    std::cout << "[ FATAL ERROR ] " << __VA_ARGS__ << "\n";                                                            \
    assert(false)
#else
#define SPARK_FATAL(...)
#endif // __SPARK_FATAL__
} // namespace Spark

#endif // CORE_LOG_H