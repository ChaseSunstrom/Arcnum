#include "window_manager.hpp"
#include "window_type.hpp"

namespace spark
{
	WindowType get_current_window_type()
	{
		auto& _window_manager = Engine::get<WindowManager>();

		return _window_manager.get_current_window_type();
	}
}