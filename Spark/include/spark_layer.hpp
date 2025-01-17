#ifndef SPARK_LAYER_HPP
#define SPARK_LAYER_HPP

#include "spark_pch.hpp"

namespace spark
{
	class Layer
	{
	public:
		Layer() = default;
		virtual ~Layer() = default;

		// On application start
		virtual void OnStart() {}
		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(f32 dt) {}
	};
}


#endif