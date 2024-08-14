#include "file_serializer.hpp"

namespace Spark
{
	FileSerializer::FileSerializer(const std::filesystem::path& path) : m_path(path)
	{
		m_output = std::ofstream(path, std::ofstream::out | std::ofstream::binary);
	}

	FileSerializer::~FileSerializer()
	{
		m_output.close();
	}

	void FileSerializer::WriteData(const char* data, size_t size)
	{
		m_output.write(data, size);
	}

	FileDeserializer::FileDeserializer(const std::filesystem::path& path) : m_path(path)
	{
		m_input = std::ifstream(path, std::ifstream::in | std::ifstream::binary);
	}

	FileDeserializer::~FileDeserializer()
	{
		m_input.close();
	}

	void FileDeserializer::ReadData(char* data, size_t size)
	{
		m_input.read(data, size);
	}
}