#ifndef SPARK_BUFFER_HPP
#define SPARK_BUFFER_HPP

#include <core/pch.hpp>
#include "vertex.hpp"

namespace Spark {
	enum class BufferUsage {
		STATIC_DRAW,
		DYNAMIC_DRAW,
		STREAM_DRAW,
		INSTANCE // Special case for instance data
	};

	enum class BufferType { VERTEX, INDEX, UNIFORM, STORAGE };

	// Abstract buffer interface
	class Buffer {
	  public:
		virtual ~Buffer()                                                         = default;
		virtual void Bind() const                                                 = 0;
		virtual void Unbind() const                                               = 0;
		virtual void SetData(const void* data, size_t size)                       = 0;
		virtual void UpdateData(const void* data, size_t size, size_t offset = 0) = 0;
	};

	class VertexBuffer : public Buffer {
	  public:
		virtual void                SetLayout(const VertexLayout& layout) = 0;
		virtual const VertexLayout& GetLayout() const                     = 0;
	};

	class IndexBuffer : public Buffer {
	  public:
		virtual size_t GetCount() const = 0;
	};

	class VertexArray {
	  public:
		virtual ~VertexArray()                                                            = default;
		virtual void               Bind() const                                           = 0;
		virtual void               Unbind() const                                         = 0;
		virtual void               AddVertexBuffer(UniquePtr<VertexBuffer> vertex_buffer) = 0;
		virtual void               SetIndexBuffer(UniquePtr<IndexBuffer> index_buffer)    = 0;
		virtual const IndexBuffer* GetIndexBuffer() const                                 = 0;
	};
}

#endif