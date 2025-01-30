#ifndef SPARK_LAYER_HPP
#define SPARK_LAYER_HPP

#include "spark_pch.hpp"

namespace spark
{
	class SPARK_API ILayer
	{
	public:
		ILayer() = default;
		virtual ~ILayer() = default;

		// On application start
		virtual void OnStart() {}
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(f32 dt) {}
	};
}


#endif