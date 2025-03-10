#ifndef SPARK_DEFINES_HPP
#define SPARK_DEFINES_HPP

#ifdef _WIN32
  #ifdef SPARK_STATIC
    // When building/using Spark as a static library, no DLL import/export decorations are needed.
    #define SPARK_API
    #define MOD_API
  #else
    // For dynamic libraries on Windows, use DLL import/export.
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
  #endif
#else
  // On non-Windows platforms, no decorations are needed.
  #define SPARK_API
  #define MOD_API
#endif

#endif // SPARK_DEFINES_HPP
