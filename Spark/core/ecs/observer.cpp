#include "observer.hpp"
#include "ecs.hpp"

namespace Spark
{
Observer::Observer(bool manual_register) : m_manual_register(manual_register)
{
    if (!manual_register)
    {
        register_observer();
    }
}

Observer::~Observer()
{
    // Automatically unregister this observer upon destruction
    ECS::get().remove_observer(*this);
}

void Observer::register_observer()
{
    ECS::get().add_observer(this);
}
} // namespace Spark