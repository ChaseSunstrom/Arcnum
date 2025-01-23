#include "impl.hpp"

extern "C" spark::IMod* CreateMod()
{
    return new TestMod();
}
