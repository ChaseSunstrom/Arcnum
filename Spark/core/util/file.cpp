#include "file.hpp"

namespace Spark
{
	std::string ReadFile(const std::filesystem::path& path)
	{
		std::ifstream file_stream;
		std::stringstream string_stream;
		file_stream.open(path);
		string_stream << file_stream.rdbuf();
		file_stream.close();
		return string_stream.str();
	}
}