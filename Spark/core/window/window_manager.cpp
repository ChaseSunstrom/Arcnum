#include "window_manager.hpp"

namespace Spark
{
namespace Internal
{
void on_api_change(std::shared_ptr<Event> event)
{
    auto api_event = std::static_pointer_cast<APIChangedEvent>(event);
    Engine::get<WindowManager>().set_window(api_event->m_api);
}
} // namespace Internal
} // namespace Spark