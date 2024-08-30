#ifndef SPARK_DEFINES_HPP
#define SPARK_DEFINES_HPP

#ifndef NDEBUG

#define __TRACE__

#endif

#if !defined(RELEASE) || defined(DEBUG)

#define __DEBUG__

#endif

#define __INFO__
#define __WARN__
#define __ERROR__

#endif