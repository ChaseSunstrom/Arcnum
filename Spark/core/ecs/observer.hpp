#ifndef SPARK_OBSERVER_HPP
#define    SPARK_OBSERVER_HPP

#include "../spark.hpp"
#include "../events/event.hpp"

namespace spark
{
	class observer
	{
	public:
		explicit observer(bool manual_register = false);

		virtual ~observer();

		virtual void on_notify(std::shared_ptr <event> event) { }

		void register_observer();

		bool m_manual_register;
	};
}

#endif