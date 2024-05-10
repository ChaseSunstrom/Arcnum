#include "api.hpp"

namespace Spark
{

void set_api(API api)
{
    g_current_api = api;
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