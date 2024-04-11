#include "file.hpp"
#include "../logging/log.hpp"

#include <boost/filesystem.hpp>

namespace spark
{
	std::string read_file(const std::filesystem::path& path)
	{
		std::string code;
		std::ifstream file;
		file.open(path);

		if (!file.is_open())
		{
			SPARK_ERROR("[FILE] Failed to open file: " << path.string());
		}

		std::stringstream stream;
		stream << file.rdbuf();
		file.close();
		code = stream.str();
		return code;
	}

	std::string get_file_extension(const std::filesystem::path& path)
	{
		return boost::filesystem::extension(path.string());
	}

	std::vector<char> read_file_to_bytes(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			SPARK_ERROR("[FILE] Failed to open file: " << path.string());
		}
		
		uint64_t file_size = static_cast<uint64_t>(file.tellg());
		std::vector<char> buffer(file_size);

		file.seekg(0);
		file.read(buffer.data(), file_size);
		file.close();
		return buffer;
	}
}