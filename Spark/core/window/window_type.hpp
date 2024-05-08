#ifndef SPARK_WINDOW_TYPE_HPP
#define SPARK_WINDOW_TYPE_HPP

#include "../spark.hpp"

namespace Spark {
enum class WindowType { UNKNOWN = 0, OPENGL, VULKAN, METAL, DIRECTX };

WindowType get_current_window_type();
} // namespace spark

#endif