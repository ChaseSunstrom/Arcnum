#ifndef SPARK_API_HPP
#define SPARK_API_HPP

#include "../spark.hpp"

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

static API g_current_api;

void set_api(API api);

API get_current_api();

bool is_current_api(API api);
} // namespace Spark
#endif