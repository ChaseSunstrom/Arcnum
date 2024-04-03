#ifndef SPARK_OBSERVER_HPP
#define	SPARK_OBSERVER_HPP

#include "../spark.hpp"
#include "../events/event.hpp"

namespace spark
{
	class observer
	{
	public:
		observer();

        virtual ~observer();

		virtual void on_notify(std::shared_ptr<event> event) {}

	};
}

#endif