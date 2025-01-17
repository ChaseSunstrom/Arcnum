#ifndef SPARK_MESH_HPP
#define SPARK_MESH_HPP

#include "spark_pch.hpp"
#include "spark_vertex.hpp"

namespace spark
{
    // Base interface for a mesh
    class IMesh
    {
    public:
        virtual ~IMesh() = default;

        virtual void SetData(const std::vector<u8>& vertex_data,
            const VertexLayout& layout,
            const std::vector<u32>& indices) = 0;

        template <typename T>
        void SetData(const std::vector<T>& vertex_data,
            const VertexLayout& layout,
            const std::vector<u32>& indices)
        {
            // Reinterpret the vector<T> data as bytes
            const auto byte_ptr = reinterpret_cast<const u8*>(vertex_data.data());
            std::vector<u8> data(byte_ptr, byte_ptr + vertex_data.size() * sizeof(T));

            // Forward to the pure virtual
            SetData(data, layout, indices);
        }

        virtual void Draw() const = 0;

    protected:
        // Some derived classes might store these, but it's optional
        std::vector<u8> m_vertex_data;
        std::vector<u32> m_index_data;
        VertexLayout     m_layout;
    };
}

#endif
