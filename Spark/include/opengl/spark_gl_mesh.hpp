// gl_mesh.hpp
#ifndef GL_MESH_HPP
#define GL_MESH_HPP

#include "spark_mesh.hpp"

namespace spark
{
	class GLMesh : public IMesh
	{
	public:
		GLMesh();
		virtual ~GLMesh() override;

		void SetData(const std::vector<u8>& vertex_data,
			const VertexLayout& layout,
			const std::vector<u32>& indices) override;

		void Draw() const override;

	private:
		u32 m_vao;
		u32 m_vbo;
		u32 m_ebo;
		usize       m_index_count;
	};
}

#endif
