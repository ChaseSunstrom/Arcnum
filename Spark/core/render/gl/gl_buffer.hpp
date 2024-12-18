#ifndef SPARK_GL_BUFFER_HPP
#define SPARK_GL_BUFFER_HPP

#include <core/render/buffer.hpp>
#include <core/util/gl.hpp>

namespace Spark {
	class GLBuffer {
	  protected:
		static GLenum GetGLUsage(BufferUsage usage) {
			switch (usage) {
				case BufferUsage::STATIC_DRAW:
					return GL_STATIC_DRAW;
				case BufferUsage::DYNAMIC_DRAW:
					return GL_DYNAMIC_DRAW;
				case BufferUsage::STREAM_DRAW:
					return GL_STREAM_DRAW;
				case BufferUsage::INSTANCE:
					return GL_DYNAMIC_DRAW;
				default:
					return GL_STATIC_DRAW;
			}
		}

		static GLenum GetGLType(BufferType type) {
			switch (type) {
				case BufferType::VERTEX:
					return GL_ARRAY_BUFFER;
				case BufferType::INDEX:
					return GL_ELEMENT_ARRAY_BUFFER;
				case BufferType::UNIFORM:
					return GL_UNIFORM_BUFFER;
				case BufferType::STORAGE:
					return GL_SHADER_STORAGE_BUFFER;
				default:
					return GL_ARRAY_BUFFER;
			}
		}
	};

	class GLVertexBuffer : public GLBuffer {
	  public:
		GLVertexBuffer(BufferUsage usage = BufferUsage::STATIC_DRAW)
			: m_usage(usage)
			, m_size(0) {
			glGenBuffers(1, &m_id);
		}

		~GLVertexBuffer() { glDeleteBuffers(1, &m_id); }

		void Bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_id); }

		void Unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

		void SetData(const void* data, size_t size) {
			m_size = size;
			Bind();
			glBufferData(GL_ARRAY_BUFFER, size, data, GetGLUsage(m_usage));
		}

		void UpdateData(const void* data, size_t size, size_t offset = 0) {
			Bind();
			glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
		}

		void SetLayout(const VertexLayout& layout) { m_layout = layout; }

		const VertexLayout& GetLayout() const { return m_layout; }

		BufferUsage GetUsage() const { return m_usage; }

		size_t GetSize() const { return m_size; }

		u32 GetID() const { return m_id; }

	  private:
		u32          m_id;
		BufferUsage  m_usage;
		size_t       m_size;
		VertexLayout m_layout;
	};

	class GLIndexBuffer : public GLBuffer {
	  public:
		GLIndexBuffer(BufferUsage usage = BufferUsage::STATIC_DRAW)
			: m_usage(usage)
			, m_count(0) {
			glGenBuffers(1, &m_id);
		}

		~GLIndexBuffer() { glDeleteBuffers(1, &m_id); }

		void Bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id); }

		void Unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

		void SetData(const void* data, size_t size) {
			Bind();
			m_count = size / sizeof(u32);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GetGLUsage(m_usage));
		}

		void UpdateData(const void* data, size_t size, size_t offset = 0) {
			Bind();
			glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
		}

		size_t GetCount() const { return m_count; }

		u32 GetID() const { return m_id; }

	  private:
		u32         m_id;
		BufferUsage m_usage;
		size_t      m_count;
	};

	class GLVertexArray {
	  public:
		GLVertexArray() { glGenVertexArrays(1, &m_id); }

		~GLVertexArray() { glDeleteVertexArrays(1, &m_id); }

		void Bind() const { glBindVertexArray(m_id); }

		void Unbind() const { glBindVertexArray(0); }

		void AddVertexBuffer(RefPtr<GLVertexBuffer> vertex_buffer) {
			Bind();
			vertex_buffer->Bind();

			const auto& layout     = vertex_buffer->GetLayout();
			const auto& attributes = layout.GetAttributes();

			for (u32 i = 0; i < attributes.Size(); i++) {
				const auto& attribute = attributes[i];
				const bool  is_matrix = attribute.type == AttributeType::MAT3 || attribute.type == AttributeType::MAT4;

				if (is_matrix) {
					SetupMatrixAttribute(attribute, layout.GetStride(), vertex_buffer->GetUsage() == BufferUsage::INSTANCE);
				} else {
					SetupAttribute(attribute, layout.GetStride(), vertex_buffer->GetUsage() == BufferUsage::INSTANCE);
				}
			}

			m_vertex_buffers.PushBack(vertex_buffer);
		}

		void SetIndexBuffer(RefPtr<GLIndexBuffer> index_buffer) {
			Bind();
			index_buffer->Bind();
			m_index_buffer = index_buffer;
		}

		RefPtr<GLIndexBuffer> GetIndexBuffer() const { return m_index_buffer; }

		const Vector<RefPtr<GLVertexBuffer>>& GetVertexBuffers() const { return m_vertex_buffers; }

		u32 GetID() const { return m_id; }

	  private:
		void SetupAttribute(const VertexAttribute& attribute, size_t stride, bool instanced) {
			glEnableVertexAttribArray(m_attribute_index);
			glVertexAttribPointer(m_attribute_index, attribute.GetComponentCount(), GetGLBaseType(attribute.type), attribute.normalized ? GL_TRUE : GL_FALSE, stride, (const void*) attribute.offset);

			if (instanced) {
				glVertexAttribDivisor(m_attribute_index, 1);
			}

			m_attribute_index++;
		}

		void SetupMatrixAttribute(const VertexAttribute& attribute, size_t stride, bool instanced) {
			const u32 count = attribute.type == AttributeType::MAT3 ? 3 : 4;

			for (u32 i = 0; i < count; i++) {
				glEnableVertexAttribArray(m_attribute_index);
				glVertexAttribPointer(m_attribute_index, count, GL_FLOAT, attribute.normalized ? GL_TRUE : GL_FALSE, stride, (const void*) (attribute.offset + sizeof(float) * count * i));

				if (instanced) {
					glVertexAttribDivisor(m_attribute_index, 1);
				}

				m_attribute_index++;
			}
		}

		static GLenum GetGLBaseType(AttributeType type) {
			switch (type) {
				case AttributeType::FLOAT:
				case AttributeType::VEC2:
				case AttributeType::VEC3:
				case AttributeType::VEC4:
				case AttributeType::MAT3:
				case AttributeType::MAT4:
					return GL_FLOAT;
				case AttributeType::INT:
				case AttributeType::IVEC2:
				case AttributeType::IVEC3:
				case AttributeType::IVEC4:
					return GL_INT;
				default:
					return GL_FLOAT;
			}
		}

	  private:
		u32                            m_id              = 0;
		u32                            m_attribute_index = 0;
		Vector<RefPtr<GLVertexBuffer>> m_vertex_buffers;
		RefPtr<GLIndexBuffer>          m_index_buffer;
	};

} // namespace Spark

#endif