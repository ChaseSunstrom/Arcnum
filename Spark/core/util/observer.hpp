#ifndef SPARK_OBSERVER_HPP
#define SPARK_OBSERVER_HPP

#include <core/pch.hpp>
#include "../event/event.hpp"

namespace Spark
{
	// Checks if a class has an AddObserver method
	template<typename T>
	concept HasAddObserver = 
	requires(T t) 
	{
		{
			t.AddObserver()
		} -> std::same_as<void>;
	};

	class IObserver
	{
	public:
		IObserver() = default;
		virtual ~IObserver() = default;

		virtual void OnNotify(const std::shared_ptr<Event> event);

		template <HasAddObserver T>
		void Create(const T& type)
		{
			T.AddObserver(this);
		}
	};
}

#endif