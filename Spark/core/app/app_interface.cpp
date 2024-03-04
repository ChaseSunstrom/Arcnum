#include "app_interface.hpp"

namespace spark
{
	std::function<void()> app_functions::s_on_start = nullptr;

	std::function<void()> app_functions::s_on_update = nullptr;

	std::function<bool(std::shared_ptr<event>)> app_functions::s_on_event = nullptr;

	void app_functions::register_functions(
			std::function<void()> on_start, std::function<void()> on_update, std::function<bool(
			std::shared_ptr<event>)> on_event)
	{
		s_on_start = on_start;
		s_on_update = on_update;
		s_on_event = on_event;
	}
}