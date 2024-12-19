#ifndef SPARK_VERTEX_ATTRIBUTE_HPP
#define SPARK_VERTEX_ATTRIBUTE_HPP

#include <core/pch.hpp>

namespace Spark {
	enum class AttributeType { FLOAT, VEC2, VEC3, VEC4, INT, IVEC2, IVEC3, IVEC4, MAT3, MAT4 };

	struct VertexAttribute {
		String        name;
		AttributeType type;
		size_t        offset;
		bool          normalized;

		VertexAttribute(const String& name, AttributeType type, size_t offset, bool normalized = false)
			: name(name)
			, type(type)
			, offset(offset)
			, normalized(normalized) {}

		size_t GetSize() const {
			switch (type) {
				case AttributeType::FLOAT:
					return sizeof(f32);
				case AttributeType::VEC2:
					return sizeof(f32) * 2;
				case AttributeType::VEC3:
					return sizeof(f32) * 3;
				case AttributeType::VEC4:
					return sizeof(f32) * 4;
				case AttributeType::INT:
					return sizeof(i32);
				case AttributeType::IVEC2:
					return sizeof(i32) * 2;
				case AttributeType::IVEC3:
					return sizeof(i32) * 3;
				case AttributeType::IVEC4:
					return sizeof(i32) * 4;
				case AttributeType::MAT3:
					return sizeof(f32) * 9;
				case AttributeType::MAT4:
					return sizeof(f32) * 16;
				default:
					return 0;
			}
		}

		u32 GetComponentCount() const {
			switch (type) {
				case AttributeType::FLOAT:
					return 1;
				case AttributeType::VEC2:
					return 2;
				case AttributeType::VEC3:
					return 3;
				case AttributeType::VEC4:
					return 4;
				case AttributeType::INT:
					return 1;
				case AttributeType::IVEC2:
					return 2;
				case AttributeType::IVEC3:
					return 3;
				case AttributeType::IVEC4:
					return 4;
				case AttributeType::MAT3:
					return 9;
				case AttributeType::MAT4:
					return 16;
				default:
					return 0;
			}
		}
	};

	class VertexLayout {
	  public:
		VertexLayout()
			: m_stride(0) {}

		template<typename T> void AddAttribute(const String& name, AttributeType type, bool normalized = false) {
			size_t offset = m_stride;
			m_attributes.EmplaceBack(name, type, offset, normalized);
			m_stride += sizeof(T);
		}

		const Vector<VertexAttribute>& GetAttributes() const { return m_attributes; }
		size_t                         GetStride() const { return m_stride; }

	  private:
		Vector<VertexAttribute> m_attributes;
		size_t                  m_stride;
	};

	class GenericVertex {
	  public:
		GenericVertex(const void* data, size_t size)
			: m_data(new u8[size])
			, m_size(size) {
			memcpy(m_data, data, size);
		}

		~GenericVertex() { delete[] m_data; }

		template<typename T> T GetAttribute(size_t offset) const { return *reinterpret_cast<const T*>(m_data + offset); }

		const void* GetData() const { return m_data; }
		size_t      GetSize() const { return m_size; }

	  private:
		u8*    m_data;
		size_t m_size;
	};
} // namespace Spark

#endif // SPARK_VERTEX_ATTRIBUTE_HPP