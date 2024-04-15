#include "window_manager.hpp"
#include "window_type.hpp"

namespace spark
{
	window_type get_current_window_type()
	{
		auto& _window_manager = engine::get<window_manager>();

		return _window_manager.get_current_window_type();
	}
}