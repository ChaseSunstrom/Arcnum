#ifndef CORE_STD_INCLUDE_HPP
#define CORE_STD_INCLUDE_HPP

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <cmath>
#include <condition_variable>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <new>
#include <numeric>
#include <queue>
#include <random>
#include <ranges>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>
#include <xutility>

#include <IRRKLANG/irrKlang.h>

namespace Spark
{
namespace internal
{
#include <GLEW/glew.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLM/common.hpp>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_inverse.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/quaternion.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <GLM/gtx/quaternion.hpp>
#include <GLM/gtx/transform.hpp>
#include <VULKAN/vulkan.h>
} // namespace internal

} // namespace Spark

#endif