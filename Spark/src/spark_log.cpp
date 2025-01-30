#include "spark_pch.hpp"
#include "spark_log.hpp"

// We only need {fmt} in the .cpp
#define FMT_UNICODE 0
#include <fmt/core.h>
#include <fmt/color.h>

#include <cstdarg>   // va_list, va_start, va_end, va_copy
#include <cstdio>    // vsnprintf
#include <cstdlib>   // malloc, free

namespace spark
{
    // Define static members
    bool Logger::s_enabled_levels[4] = { true, true, true, true };
    LogLevel Logger::s_current_level = LogLevel::DEBUG;

    //------------------------------------------------------------------------------
    // Helper: map LogLevel to a fmt::text_style (color, bold, etc.)
    //------------------------------------------------------------------------------
    static fmt::text_style GetLogColor(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return fg(fmt::color::blue_violet); // Blue-ish for debug
        case LogLevel::INFO:
            return fg(fmt::color::green);      // Green for info
        case LogLevel::WARN:
            return fg(fmt::color::yellow);     // Yellow for warn
        case LogLevel::ERROR:
            return fg(fmt::color::red);        // Red for error
        }
        return fg(fmt::color::white);
    }

    //------------------------------------------------------------------------------
    // Helper: map LogLevel to a string prefix, e.g., "[DEBUG] "
    //------------------------------------------------------------------------------
    static const char* GetLogPrefix(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG: return "[DEBUG] ";
        case LogLevel::INFO:  return "[INFO] ";
        case LogLevel::WARN:  return "[WARN] ";
        case LogLevel::ERROR: return "[ERROR] ";
        }
        return "[UNKNOWN] ";
    }

    //------------------------------------------------------------------------------
    // Disable logging for a given level
    //------------------------------------------------------------------------------
    void Logger::Disable(LogLevel level)
    {
        s_enabled_levels[(int)level] = false;
    }

    //------------------------------------------------------------------------------
    // Internal function to handle varargs => dynamic buffer => return char*
    //------------------------------------------------------------------------------
    static char* FormatString(const char* format, va_list args)
    {
        // We first copy the va_list to compute the size needed
        va_list copy;
        va_copy(copy, args);

        // vsnprintf with nullptr counts how many bytes we need
        int len = vsnprintf(nullptr, 0, format, copy);
        va_end(copy);

        if (len < 0)
        {
            // vsnprintf error, return an empty string
            char* error_str = (char*)malloc(1);
            error_str[0] = '\0';
            return error_str;
        }

        // Allocate enough space (+1 for null terminator)
        char* buffer = (char*)malloc(len + 1);
        if (!buffer)
        {
            // Allocation failed
            char* error_str = (char*)malloc(1);
            error_str[0] = '\0';
            return error_str;
        }

        // Now do the actual vsnprintf into our buffer
        vsnprintf(buffer, len + 1, format, args);
        return buffer;
    }

    //------------------------------------------------------------------------------
    // Log() with current level (no newline)
    //------------------------------------------------------------------------------
    void Logger::Log(const char* format, ...)
    {
        if (!s_enabled_levels[(int)s_current_level])
            return;

        va_list args;
        va_start(args, format);

        char* buffer = FormatString(format, args);

        va_end(args);

        auto color_style = GetLogColor(s_current_level);
        auto prefix = GetLogPrefix(s_current_level);
        // Example: "[DEBUG] Hello World"
        fmt::print(color_style, "{}{}", prefix, buffer);

        free(buffer);
    }

    // Logln() with current level (with newline)
    void Logger::Logln(const char* format, ...)
    {
        if (!s_enabled_levels[(int)s_current_level])
            return;

        va_list args;
        va_start(args, format);

        char* buffer = FormatString(format, args);

        va_end(args);

        auto color_style = GetLogColor(s_current_level);
        auto prefix = GetLogPrefix(s_current_level);
        // We add our own "\n"
        fmt::print(color_style, "{}{}\n", prefix, buffer);

        free(buffer);
    }

    //------------------------------------------------------------------------------
    // Log(LogLevel, ...) (no newline)
    //------------------------------------------------------------------------------
    void Logger::Log(LogLevel level, const char* format, ...)
    {
        if (!s_enabled_levels[(int)level])
            return;

        s_current_level = level; // update current level if needed

        va_list args;
        va_start(args, format);

        char* buffer = FormatString(format, args);

        va_end(args);

        auto color_style = GetLogColor(level);
        auto prefix = GetLogPrefix(level);

        fmt::print(color_style, "{}{}", prefix, buffer);

        free(buffer);
    }

    // Logln(LogLevel, ...) (with newline)
    void Logger::Logln(LogLevel level, const char* format, ...)
    {
        if (!s_enabled_levels[(int)level])
            return;

        s_current_level = level;

        va_list args;
        va_start(args, format);

        char* buffer = FormatString(format, args);

        va_end(args);

        auto color_style = GetLogColor(level);
        auto prefix = GetLogPrefix(level);

        fmt::print(color_style, "{}{}\n", prefix, buffer);

        free(buffer);
    }

    void Logger::Log(LogLevel level, const std::string& message) {
        if (s_enabled_levels[static_cast<int>(level)]) {
            std::cout << GetLogPrefix(level) << message;
        }
    }

}
