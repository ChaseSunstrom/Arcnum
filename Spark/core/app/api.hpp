#ifndef SPARK_API_HPP
#define SPARK_API_HPP

#include "../spark.hpp"
#include "../events/event.hpp"

namespace Spark
{
enum class API
{
    UNKNOWN = 0,
    OPENGL,
    VULKAN,
    METAL,
    DIRECTX,
};

struct APIChangedEvent : Event
{
    APIChangedEvent(API api) : Event(API_CHANGED_EVENT), m_api(api)
    {
	}

	API m_api;
};

static API g_current_api = API::UNKNOWN;

void set_api(API api);

API get_current_api();

bool is_current_api(API api);
} // namespace Spark
#endif