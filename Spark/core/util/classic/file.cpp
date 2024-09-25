#include "file.hpp"
#include <fstream>


namespace Spark {
	String ReadFile(const std::filesystem::path& path) {
		std::ifstream     file_stream;
		std::stringstream string_stream;
		file_stream.open(path);
		string_stream << file_stream.rdbuf();
		file_stream.close();
		String str = string_stream.str();
		return str;
	}
} // namespace Spark