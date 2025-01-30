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

        static void Log(LogLevel level, const std::string& message);

    private:
        // Store if logging is enabled for each level
        // For example, we can just store an array indexed by (int)LogLevel.
        static bool s_enabled_levels[4];

        // Current "active" log level if not explicitly specified
        static LogLevel s_current_level;
    };

    class LoggerStream {
    public:
        LoggerStream(LogLevel level = LogLevel::INFO)
            : m_level(level), m_buffer() {}

        ~LoggerStream() {
            Logger::Log(m_level, m_buffer.str().c_str());
        }

        // Overload the << operator for various types
        template <typename T>
        LoggerStream& operator<<(const T& value) {
            m_buffer << value;
            return *this;
        }

        // Overload for manipulators like std::endl
        typedef std::ostream& (*Manipulator)(std::ostream&);
        LoggerStream& operator<<(Manipulator manip) {
            manip(m_buffer);
            return *this;
        }

    private:
        LogLevel m_level;
        std::ostringstream m_buffer;
    };

    // Helper function to create a LoggerStream with a specific log level
    inline LoggerStream Log(LogLevel level = LogLevel::INFO) {
        return LoggerStream(level);
    }

}

#endif // SPARK_LOG_HPP
