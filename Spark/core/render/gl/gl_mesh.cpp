#include "gl_mesh.hpp"
#include <core/util/gl.hpp>

namespace Spark {
	static void GLCreateMesh(Vector<Vertex>& vertices, Vector<u32>& indices, u32& vao, u32& vbo, u32& ebo, u32 draw_type) {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.Size() * sizeof(Vertex), vertices.Data(), draw_type);

		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.Size() * sizeof(u32), indices.Data(), draw_type);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texcoords));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	template<typename _Ty> static void GLUpdateMesh(Vector<_Ty>& old_data, const Vector<_Ty>& new_data, u32 bo) {
		glBindBuffer(GL_ARRAY_BUFFER, bo);

		if (new_data.Size() != old_data.Size()) {
			// Resize necessary, use glBufferData
			glBufferData(GL_ARRAY_BUFFER, new_data.Size() * sizeof(_Ty), new_data.Data(), GL_DYNAMIC_DRAW);
		} else {
			// Same size, use glBufferSubData for potentially better performance
			glBufferSubData(GL_ARRAY_BUFFER, 0, new_data.Size() * sizeof(_Ty), new_data.Data());
		}

		old_data = new_data;
	}

	void GLStaticMesh::CreateMesh() { GLCreateMesh(m_vertices, m_indices, m_vao, m_vbo, m_ebo, GL_STATIC_DRAW); }

	void GLDynamicMesh::CreateMesh() { GLCreateMesh(m_vertices, m_indices, m_vao, m_vbo, m_ebo, GL_DYNAMIC_DRAW); }

	void GLDynamicMesh::UpdateMesh(const Vector<Vertex>& new_vertices) { GLUpdateMesh<Vertex>(m_vertices, new_vertices, m_vbo); }

	void GLDynamicMesh::UpdateIndices(const Vector<u32>& new_indices) { GLUpdateMesh<u32>(m_indices, new_indices, m_ebo); }

	void GLDynamicMesh::Update(const Vector<Vertex>& vertices, const Vector<u32>& indices) {
		UpdateMesh(vertices);
		UpdateIndices(indices);
	}
} // namespace Spark