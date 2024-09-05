#ifndef SPARK_GL_MESH_HPP
#define SPARK_GL_MESH_HPP

#include <core/pch.hpp>
#include <core/render/mesh.hpp>
#include <core/system/manager.hpp>

namespace Spark {
	class GLStaticMesh : public StaticMesh {
	  public:
		void CreateMesh() override;

		u32 GetVAO() const { return m_vao; }
		u32 GetVBO() const { return m_vbo; }
		u32 GetEBO() const { return m_ebo; }

	  private:
		GLStaticMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices)
			: StaticMesh(vertices, indices) {}


	  private:
		u32 m_vao = 0;
		u32 m_vbo = 0;
		u32 m_ebo = 0;
	};

	class GLDynamicMesh : public DynamicMesh {
	  public:
		void CreateMesh() override;
		void Update(const std::vector<Vertex>& vertices, const std::vector<u32>& indices) override;
		void UpdateMesh(const std::vector<Vertex>& vertices) override;
		void UpdateIndices(const std::vector<u32>& indices) override;

		u32 GetVAO() const { return m_vao; }
		u32 GetVBO() const { return m_vbo; }
		u32 GetEBO() const { return m_ebo; }
	  private:
		GLDynamicMesh(const std::vector<Vertex>& vertices, const std::vector<u32>& indices)
			: DynamicMesh(vertices, indices) {}

	  private:
		u32 m_vao = 0;
		u32 m_vbo = 0;
		u32 m_ebo = 0;
	};
} // namespace Spark

#endif