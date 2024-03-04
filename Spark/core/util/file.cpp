#include "file.hpp"

namespace spark
{
	std::string read_file(const std::filesystem::path& path)
	{
		std::string code;
		std::ifstream file_stream;
		file_stream.open(path);
		std::stringstream stream;
		stream << file_stream.rdbuf();
		file_stream.close();
		code = stream.str();
		return code;
	}
}