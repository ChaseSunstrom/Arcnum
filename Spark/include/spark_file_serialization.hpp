#ifndef SPARK_FILE_SERIALIZER_HPP
#define SPARK_FILE_SERIALIZER_HPP

#include "spark_serialization.hpp"

namespace spark {
	class SPARK_API FileSerializer : public ISerializer {
	public:
		FileSerializer(const std::filesystem::path& path);
		~FileSerializer();
		void WriteData(const char* data, usize  size) override;

	private:
		std::filesystem::path m_path;
		std::ofstream         m_output;
	};

	class SPARK_API FileDeserializer : public IDeserializer {
	public:
		FileDeserializer(const std::filesystem::path& path);
		~FileDeserializer();
		void ReadData(char* data, usize  size) override;

	private:
		std::filesystem::path m_path;
		std::ifstream         m_input;
	};
} // namespace Spark

#endif