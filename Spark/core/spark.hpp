#ifndef SPARK_HPP
#define SPARK_HPP

#define __SPARK_TEST__

#define __SPARK_DEBUG__

#define __SPARK_TRACE__
#define __SPARK_INFO__
#define __SPARK_WARN__
#define __SPARK_ERROR__
#define __SPARK_FATAL__

#define __SPARK_ASSERT__(_expression) _expression ? 1 : 0

#include "std_include.hpp"
#include "types.hpp"
#include "Engine.hpp"

#include "util/macros.hpp"

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

namespace spark
{
	namespace math = internal::glm;

	namespace internal
	{
		using namespace irrklang;
	}
}

#endif // CORE_CORE_H