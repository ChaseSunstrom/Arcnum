#ifndef SPARK_LAYER_HPP
#define SPARK_LAYER_HPP

#include "../spark.hpp"

#include "../events/event.hpp"

namespace spark
{
	// ==============================================================================
	// LAYER:        | Used for organizing related data in the layer stack and application
	// ==============================================================================

	class layer
	{
	public:
		virtual ~layer() = default;

		virtual void on_attach() = 0;

		virtual void on_detach() = 0;

		virtual void on_update() = 0;

		virtual void on_event(std::shared_ptr <event>) = 0;
	};
}

#endif //CORE_LAYER_H