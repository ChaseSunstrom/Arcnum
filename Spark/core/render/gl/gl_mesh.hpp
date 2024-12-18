#ifndef SPARK_GL_MESH_HPP
#define SPARK_GL_MESH_HPP

#include <core/pch.hpp>
#include <core/render/mesh.hpp>
#include "gl_buffer.hpp"

namespace Spark {
	class GLGenericMesh : public GenericMesh {
	  public:
		GLGenericMesh(const String& name, BufferUsage usage = BufferUsage::STATIC_DRAW)
			: m_name(name)
			, m_vertex_array(MakeUnique<GLVertexArray>())
			, m_vertex_usage(usage) {}

		virtual ~GLGenericMesh() = default;

		void Bind() const override { m_vertex_array->Bind(); }

		void Unbind() const override { m_vertex_array->Unbind(); }

		template<typename T> void SetVertexData(const Vector<T>& vertices, const VertexLayout& layout) { SetVertexData(vertices.Data(), vertices.Size() * sizeof(T), layout); }

		// TODO: FIX LEAKS HERE
		// {
		void SetVertexData(const void* data, size_t size, const VertexLayout& layout) override {
			auto vbo           = new GLVertexBuffer(m_vertex_usage);
			auto vertex_buffer = MakeRefPtr<GLVertexBuffer>(vbo); // Create RefPtr correctly
			vertex_buffer->SetLayout(layout);
			vertex_buffer->SetData(data, size);
			m_vertex_array->AddVertexBuffer(vertex_buffer);
			m_vertex_count = size / layout.GetStride();
		}

		void SetIndexData(const Vector<u32>& indices) override {
			auto ibo          = new GLIndexBuffer(m_vertex_usage);
			auto index_buffer = MakeRefPtr<GLIndexBuffer>(ibo); // Create RefPtr correctly
			index_buffer->SetData(indices.Data(), indices.Size() * sizeof(u32));
			m_vertex_array->SetIndexBuffer(index_buffer);
		}

		void SetInstanceData(const void* data, size_t size, const VertexLayout& layout) override {
			auto vbo             = new GLVertexBuffer(BufferUsage::INSTANCE);
			auto instance_buffer = MakeRefPtr<GLVertexBuffer>(vbo); // Create RefPtr correctly
			instance_buffer->SetLayout(layout);
			instance_buffer->SetData(data, size);
			m_vertex_array->AddVertexBuffer(instance_buffer);
		}
		// }

		template<typename T> void SetInstanceData(const Vector<T>& instances, const VertexLayout& layout) { SetInstanceData(instances.Data(), instances.Size() * sizeof(T), layout); }

		void Draw() const override {
			Bind();
			if (m_vertex_array->GetIndexBuffer()) {
				glDrawElements(GL_TRIANGLES, m_vertex_array->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
			} else {
				glDrawArrays(GL_TRIANGLES, 0, m_vertex_count);
			}
			Unbind();
		}

		void DrawInstanced(u32 instance_count) const override {
			Bind();
			if (m_vertex_array->GetIndexBuffer()) {
				glDrawElementsInstanced(GL_TRIANGLES, m_vertex_array->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr, instance_count);
			} else {
				glDrawArraysInstanced(GL_TRIANGLES, 0, m_vertex_count, instance_count);
			}
			Unbind();
		}

	  private:
		String                   m_name;
		UniquePtr<GLVertexArray> m_vertex_array;
		BufferUsage              m_vertex_usage;
		size_t                   m_vertex_count = 0;
	};
} // namespace Spark

#endif