#ifndef SPARK_SYSTEM_HPP
#define SPARK_SYSTEM_HPP

#include <core/pch.hpp>

namespace Spark
{
	class ISystem
	{
	public:
		ISystem() = default;
		virtual ~ISystem() = default;

		virtual void Start() = 0;
		virtual void Update() = 0;
		virtual void Shutdown() = 0;
	};
}


#endif