#ifndef SPARK_DEFINES_HPP
#define SPARK_DEFINES_HPP

#ifdef _WIN32
    #ifdef SPARK_EXPORTS
        #define SPARK_API __declspec(dllexport)
    #else
        #define SPARK_API __declspec(dllimport)
    #endif
    
    #ifdef MOD_EXPORTS
        #define MOD_API __declspec(dllexport)
    #else
        #define MOD_API __declspec(dllimport)
    #endif
#else
    // For non-Windows platforms
    #define SPARK_API
    #define MOD_API
#endif

#endif
