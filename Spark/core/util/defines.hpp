#ifndef SPARK_DEFINES_HPP
#define SPARK_DEFINES_HPP

#if (!defined(RELEASE) && !defined(__DIST__) && !defined(NDEBUG)) || defined(DEBUG)
#	define __DEBUG__
#endif

#ifndef __DIST__
#	ifndef NDEBUG
#		define __TRACE__
#	endif

#	define __INFO__
#	define __WARN__
#	define __ERROR__
#	define __FATAL__
#endif

#define assert_false assert(false)

#define _SPARK ::Spark::
#define _MATH ::Spark::Math:: 

#endif