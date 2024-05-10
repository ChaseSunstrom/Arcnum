#ifndef SPARK_DIRECTX_WINDOW_HPP
#define SPARK_DIRECTX_WINDOW_HPP

#include "../window.hpp"

namespace Spark
{
class DirectXWindow : Window, Singleton<DirectXWindow>
{
};
} // namespace Spark

#endif