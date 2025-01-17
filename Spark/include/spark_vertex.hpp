#ifndef SPARK_VERTEX_HPP
#define SPARK_VERTEX_HPP

#include "spark_pch.hpp"

namespace spark
{
	enum class AttributeType { FLOAT, DOUBLE, VEC2, VEC3, VEC4, INT, IVEC2, IVEC3, IVEC4, MAT3, MAT4 };

	struct VertexAttribute {
		std::string   name;
		AttributeType type;
		size_t        offset;
		bool          normalized;

		VertexAttribute(const std::string& name, AttributeType type, size_t offset, bool normalized = false)
			: name(name)
			, type(type)
			, offset(offset)
			, normalized(normalized) {}

		size_t GetSize() const {
			switch (type) {
			case AttributeType::FLOAT:
				return sizeof(f32);
			case AttributeType::DOUBLE:
				return sizeof(f64);
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

		template<typename T> constexpr void AddAttribute(const std::string& name, AttributeType type, bool normalized = false) {
			size_t offset = m_stride;
			m_attributes.emplace_back(name, type, offset, normalized);
			m_stride += sizeof(T);
		}

		constexpr void AddAttribute(const std::string_view name,
			AttributeType type,
			size_t field_size,
			bool normalized = false) {
			size_t offset = m_stride;
			m_attributes.emplace_back(std::string(name), type, offset, normalized);
			m_stride += field_size;
		}

		const std::vector<VertexAttribute>& GetAttributes() const { return m_attributes; }
		size_t                         GetStride() const { return m_stride; }

	private:
		std::vector<VertexAttribute> m_attributes;
		size_t                  m_stride;
	};

	struct LayoutDescriptor
	{
		const char*   name;
		AttributeType type;
		usize         field_size;   // e.g., sizeof(glm::vec3)
		bool          normalized = false;

		constexpr LayoutDescriptor(const char* n,
			AttributeType t,
			usize fs,
			bool norm = false)
			: name(n)
			, type(t)
			, field_size(fs)
			, normalized(norm)
		{ }
	};


	template <typename T, LayoutDescriptor... Descriptors>
	inline VertexLayout& CreateVertexLayout()
	{
		static VertexLayout s_layout;
		static bool s_initialized = false;

		if (!s_initialized)
		{
			// Expand the Descriptors pack, adding each attribute
			((s_layout.AddAttribute(
				Descriptors.name,
				Descriptors.type,
				Descriptors.field_size,
				Descriptors.normalized
			)), ...);

			s_initialized = true;
		}

		return s_layout;
	}
	
	template <typename T, LayoutDescriptor... Descriptors>
	inline const VertexLayout& GetVertexLayout()
	{
		return CreateVertexLayout<T, Descriptors...>();
	}
}

#endif