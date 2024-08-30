#include "gl_mesh.hpp"
#include <core/util/gl.hpp>

namespace Spark {
static void GLCreateMesh(std::vector<Vertex>& vertices, std::vector<u32>& indices, u32& vao, u32& vbo, u32& ebo, u32 draw_type) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), draw_type);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(u32), indices.data(), draw_type);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texcoords));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
}

template <typename T> static void GLUpdateMesh(std::vector<T>& old_data, const std::vector<T>& new_data, u32 bo) {
	glBindBuffer(GL_ARRAY_BUFFER, bo);

	if (new_data.size() != old_data.size()) {
		// Resize necessary, use glBufferData
		glBufferData(GL_ARRAY_BUFFER, new_data.size() * sizeof(T), new_data.data(), GL_DYNAMIC_DRAW);
	} else {
		// Same size, use glBufferSubData for potentially better performance
		glBufferSubData(GL_ARRAY_BUFFER, 0, new_data.size() * sizeof(T), new_data.data());
	}

	old_data = new_data;
}

void GLStaticMesh::CreateMesh() { GLCreateMesh(m_vertices, m_indices, m_vao, m_vbo, m_ebo, GL_STATIC_DRAW); }

void GLDynamicMesh::CreateMesh() { GLCreateMesh(m_vertices, m_indices, m_vao, m_vbo, m_ebo, GL_DYNAMIC_DRAW); }

void GLDynamicMesh::UpdateMesh(const std::vector<Vertex>& new_vertices) { GLUpdateMesh<Vertex>(m_vertices, new_vertices, m_vbo); }

void GLDynamicMesh::UpdateIndices(const std::vector<u32>& new_indices) { GLUpdateMesh<u32>(m_indices, new_indices, m_ebo); }

void GLDynamicMesh::Update(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) {
	UpdateMesh(vertices);
	UpdateIndices(indices);
}
} // namespace Spark