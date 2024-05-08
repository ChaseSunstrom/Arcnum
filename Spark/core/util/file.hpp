#ifndef SPARK_FILE_HPP
#define SPARK_FILE_HPP

#include "../spark.hpp"

namespace Spark {
std::string read_file(const std::filesystem::path &path);

std::string get_file_extension(const std::filesystem::path &path);

std::vector<char> read_file_to_bytes(const std::filesystem::path &path);
} // namespace spark

#endif