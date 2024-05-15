#ifndef SPARK_METAL_WINDOW_HPP
#define SPARK_METAL_WINDOW_HPP

#include "../window.hpp"
#include "../../util/singleton.hpp"

namespace Spark
{
class MetalWindow : Window, Singleton<MetalWindow>
{
};
} // namespace Spark

#endif