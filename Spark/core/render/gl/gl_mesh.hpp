#ifndef SPARK_GL_MESH_HPP
#define SPARK_GL_MESH_HPP

#include <core/pch.hpp>
#include <core/render/mesh.hpp>
#include <core/system/manager.hpp>

namespace Spark {
	class GLStaticMesh : public StaticMesh {
	  public:
		friend class Manager<StaticMesh>;
		void CreateMesh() override;

		u32 GetVAO() const { return m_vao; }
		u32 GetVBO() const { return m_vbo; }
		u32 GetEBO() const { return m_ebo; }

	  private:
		GLStaticMesh(const Vector<Vertex>& vertices, const Vector<u32>& indices = {})
			: StaticMesh(vertices, indices) {}


	  private:
		u32 m_vao = 0;
		u32 m_vbo = 0;
		u32 m_ebo = 0;
	};

	class GLDynamicMesh : public DynamicMesh {
	  public:
		friend class Manager<DynamicMesh>;
		void CreateMesh() override;
		void Update(const Vector<Vertex>& vertices, const Vector<u32>& indices) override;
		void UpdateMesh(const Vector<Vertex>& vertices) override;
		void UpdateIndices(const Vector<u32>& indices) override;

		u32 GetVAO() const { return m_vao; }
		u32 GetVBO() const { return m_vbo; }
		u32 GetEBO() const { return m_ebo; }
	  private:
		GLDynamicMesh(const Vector<Vertex>& vertices, const Vector<u32>& indices)
			: DynamicMesh(vertices, indices) {}

	  private:
		u32 m_vao = 0;
		u32 m_vbo = 0;
		u32 m_ebo = 0;
	};
} // namespace Spark

#endif