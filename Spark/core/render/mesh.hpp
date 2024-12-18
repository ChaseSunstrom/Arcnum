#ifndef SPARK_MESH_HPP
#define SPARK_MESH_HPP

#include <core/pch.hpp>
#include <core/system/manager.hpp>
#include "vertex.hpp"

namespace Spark {
	class GenericMesh {
	  public:
		virtual ~GenericMesh()                               = default;

		virtual void Bind() const                            = 0;
		virtual void Unbind() const                          = 0;
		virtual void Draw() const                            = 0;
		virtual void DrawInstanced(u32 instance_count) const = 0;

		template<typename T> void SetVertexData(const Vector<T>& vertices, const VertexLayout& layout) { SetVertexData(vertices.Data(), vertices.Size() * sizeof(T), layout); }

		virtual void SetVertexData(const void* data, size_t size, const VertexLayout& layout)                = 0;
		virtual void SetIndexData(const Vector<u32>& indices)                                                = 0;

		// Optional instance data support
		virtual void              SetInstanceData(const void* data, size_t size, const VertexLayout& layout) = 0;
		template<typename T> void SetInstanceData(const Vector<T>& instances, const VertexLayout& layout) { SetInstanceData(instances.Data(), instances.Size() * sizeof(T), layout); }
	};
} // namespace Spark

#endif // SPARK_MESH_HPP