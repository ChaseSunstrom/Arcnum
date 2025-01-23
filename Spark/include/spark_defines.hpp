#ifndef SPARK_DEFINES_HPP
#define SPARK_DEFINES_HPP

#ifdef SPARK_EXPORTS
#define SPARK_API __declspec(dllexport)
#else
#define SPARK_API __declspec(dllimport)
#endif


#endif