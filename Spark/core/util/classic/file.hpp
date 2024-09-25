#ifndef SPARK_FILE_HPP
#define SPARK_FILE_HPP

#include <core/util/container/string.hpp>
#include <filesystem>
namespace Spark {
	/**
	 * @brief Reads the contents of a file and returns them as a string
	 * @param path The path to the file to read
	 * @return The contents of the file as a string
	 */
	String ReadFile(const std::filesystem::path& path);
}

#endif