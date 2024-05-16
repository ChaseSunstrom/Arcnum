#include "api.hpp"
#include "../events/sub.hpp"

namespace Spark
{

void set_api(API api)
{
    g_current_api = api;

    publish_to_topic(API_CHANGED_RECEIVE_TOPIC, std::make_shared<APIChangedEvent>(api));
}

API get_current_api()
{
    return g_current_api;
}

bool is_current_api(API api)
{
    return get_current_api() == api;
}
} // namespace Spark