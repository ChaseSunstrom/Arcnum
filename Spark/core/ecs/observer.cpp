#include "observer.hpp"
#include "ecs.hpp"

namespace spark
{
	observer::observer(bool manual_register) :
		m_manual_register(manual_register)
	{
		if (!manual_register)
		{
			register_observer();
		}
	}

	observer::~observer()
	{
		// Automatically unregister this observer upon destruction
		ecs::get().remove_observer(*this);
	}

	void observer::register_observer()
	{
		ecs::get().add_observer(this);
	}
}