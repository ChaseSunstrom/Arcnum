#ifndef SPARK_FILE_HPP
#define SPARK_FILE_HPP

#include "../spark.hpp"

namespace spark
{
	std::string read_file(const std::filesystem::path& path);
}

#endif