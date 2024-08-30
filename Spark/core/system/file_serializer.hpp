#ifndef SPARK_FILE_SERIALIZER_HPP
#define SPARK_FILE_SERIALIZER_HPP

#include "serializer.hpp"

namespace Spark {
class FileSerializer : public ISerializer {
  public:
	FileSerializer(const std::filesystem::path& path);
	~FileSerializer();
	void WriteData(const char* data, size_t size) override;

  private:
	std::filesystem::path m_path;
	std::ofstream m_output;
};

class FileDeserializer : public IDeserializer {
  public:
	FileDeserializer(const std::filesystem::path& path);
	~FileDeserializer();
	void ReadData(char* data, size_t size) override;

  private:
	std::filesystem::path m_path;
	std::ifstream m_input;
};
} // namespace Spark

#endif