#ifndef SPARK_OBSERVER_HPP
#define	SPARK_OBSERVER_HPP

#include "../spark.hpp"
#include "../events/event.hpp"

namespace spark
{
	class observer
	{
	public:
        observer()
        {
            // Automatically register this observer with the ecs system upon creation
            ecs::get().add_observer(this);
        }

        virtual ~observer()
        {
            // Automatically unregister this observer upon destruction
            ecs::get().remove_observer(*this);
        }

		virtual void on_notify(std::shared_ptr<event> event);
	};
}

#endif