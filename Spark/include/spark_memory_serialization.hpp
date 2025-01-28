#ifndef SPARK_MEMORY_SERIALIZER_HPP
#define SPARK_MEMORY_SERIALIZER_HPP

#include "spark_serialization.hpp"
#include <vector>
#include <cstring> // For std::memcpy

namespace spark {

    class SPARK_API MemorySerializer : public ISerializer {
    public:
        MemorySerializer();
        ~MemorySerializer();

        void WriteData(const char* data, usize size) override;
        const std::vector<char>& GetBuffer() const { return m_buffer; }
        void Clear() { m_buffer.clear(); }

    private:
        std::vector<char> m_buffer; /**< Internal buffer storing serialized data. */
    };

    class SPARK_API MemoryDeserializer : public IDeserializer {
    public:
        MemoryDeserializer(const std::vector<char>& buffer);
        ~MemoryDeserializer();
        void ReadData(char* data, usize size) override;
        void Reset() { m_position = 0; }
    private:
        const std::vector<char>& m_buffer; /**< Reference to the buffer to read from. */
        usize m_position;                  /**< Current read position within the buffer. */
    };
} // namespace spark

#endif // SPARK_MEMORY_SERIALIZER_HPP
