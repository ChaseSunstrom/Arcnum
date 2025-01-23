#ifndef SPARK_LOG_HPP
#define SPARK_LOG_HPP

#include <cstdarg> // for va_list

#ifdef DEBUG
#undef DEBUG
#endif

#ifdef ERROR
#undef ERROR
#endif

namespace spark
{
    // Simple log levels
    enum class LogLevel
    {
        DEBUG,
        INFO,
        WARN,
        ERROR,
    };

    class SPARK_API Logger
    {
    public:
        // Disable logging for a particular level
        static void Disable(LogLevel level);

        // Variadic C-style logging (current log level)
        static void Log(const char* format, ...);
        static void Logln(const char* format, ...);

        // Variadic C-style logging (explicit log level)
        static void Log(LogLevel level, const char* format, ...);
        static void Logln(LogLevel level, const char* format, ...);

    private:
        // Store if logging is enabled for each level
        // For example, we can just store an array indexed by (int)LogLevel.
        static bool s_enabled_levels[4];

        // Current "active" log level if not explicitly specified
        static LogLevel s_current_level;
    };
}

#endif // SPARK_LOG_HPP
