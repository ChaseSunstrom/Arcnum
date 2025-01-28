#include "spark_pch.hpp"
#include "spark_memory_serialization.hpp"
#include "spark_log.hpp"

namespace spark {
    MemorySerializer::MemorySerializer()
        : m_buffer() { }

    MemorySerializer::~MemorySerializer() { }

    void MemorySerializer::WriteData(const char* data, usize size) {
        m_buffer.insert(m_buffer.end(), data, data + size);
    }

    MemoryDeserializer::MemoryDeserializer(const std::vector<char>& buffer)
        : m_buffer(buffer), m_position(0) { }

    MemoryDeserializer::~MemoryDeserializer() { }

    void MemoryDeserializer::ReadData(char* data, usize size) {
        // Check if there is enough data left to read
        if (m_position + size > m_buffer.size()) {
            Logger::Logln(LogLevel::ERROR, "MemoryDeserializer: Attempt to read beyond buffer size.");
            assert(false);
        }

        // Copy data from the buffer to the provided memory location
        std::memcpy(data, m_buffer.data() + m_position, size);
        m_position += size;
    }

} // namespace spark
