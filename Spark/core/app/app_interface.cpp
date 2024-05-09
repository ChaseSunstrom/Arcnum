#include "app_interface.hpp"

namespace Spark
{
std::function<void()> AppFunctions::s_on_start = nullptr;

std::function<void()> AppFunctions::s_on_update = nullptr;

std::function<bool(std::shared_ptr<Event>)> AppFunctions::s_on_event = nullptr;

void AppFunctions::register_functions(std::function<void()> on_start, std::function<void()> on_update,
                                      std::function<bool(std::shared_ptr<Event>)> on_event)
{
    s_on_start = on_start;
    s_on_update = on_update;
    s_on_event = on_event;
}
} // namespace Spark