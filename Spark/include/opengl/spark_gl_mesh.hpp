#ifndef GL_MESH_HPP
#define GL_MESH_HPP

#include "spark_math_types.hpp"
#include "spark_mesh.hpp"
#include "spark_transform.hpp"

namespace spark::opengl
{
    class SPARK_API GLMesh : public IMesh
    {
    public:
        GLMesh();
        virtual ~GLMesh() override;

        void SetDataBytes(const std::vector<u8>& vertex_data,
            const VertexLayout& layout,
            const std::vector<u32>& indices) override;


        void SetInstanceData(const std::vector<math::Mat4>& instance_data) override;

        void Draw() const override;
        void DrawInstanced(usize instance_count) const override;

    private:
        u32 m_vao;
        u32 m_vbo;
        u32 m_ebo;
        u32 m_instance_vbo;
        usize m_index_count;
    };
}

#endif
