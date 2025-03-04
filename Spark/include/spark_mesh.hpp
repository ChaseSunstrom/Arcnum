#ifndef SPARK_MESH_HPP
#define SPARK_MESH_HPP

#include "spark_pch.hpp"

#include "spark_transform.hpp"
#include "spark_vertex.hpp"

namespace spark
{
    // Base interface for a mesh
    class SPARK_API IMesh
    {
    public:
        virtual ~IMesh() = default;

        virtual void SetDataBytes(const std::vector<u8>& vertex_data,
            const VertexLayout& layout,
            const std::vector<u32>& indices = {}) = 0;

        template <typename T>
        void SetData(const std::vector<T>& vertex_data,
            const VertexLayout& layout,
            const std::vector<u32>& indices = {})
        {
            const auto byte_ptr = reinterpret_cast<const u8*>(vertex_data.data());
            std::vector<u8> data(byte_ptr, byte_ptr + vertex_data.size() * sizeof(T));
            SetDataBytes(data, layout, indices);
        }

        virtual void SetInstanceData(const std::vector<math::Mat4>& instance_data) = 0;
        virtual void Draw() const = 0;
        virtual void DrawInstanced(usize instance_count) const = 0;

    protected:
        // Some derived classes might store these, but it's optional
        std::vector<u8> m_vertex_data;
        std::vector<u32> m_index_data;
        VertexLayout     m_layout;
    };
}

#endif
