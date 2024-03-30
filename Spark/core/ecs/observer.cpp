#include "observer.hpp"
#include "ecs.hpp"

namespace spark
{
    observer::observer()
    {
        // Automatically register this observer with the ecs system upon creation
        ecs::get().add_observer(this);
    }

    observer::~observer()
    {
        // Automatically unregister this observer upon destruction
        ecs::get().remove_observer(*this);
    }
}