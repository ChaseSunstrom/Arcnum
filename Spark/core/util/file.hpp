#ifndef SPARK_FILE_HPP
#define SPARK_FILE_HPP

#include <core/pch.hpp>

namespace Spark {
std::string ReadFile(const std::filesystem::path& path);
}

#endif