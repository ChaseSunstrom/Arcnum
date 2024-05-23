#ifndef SPARK_DIRECTX_MESH_HPP
#define SPARK_DIRECTX_MESH_HPP

#include "../../ecs/component/mesh.hpp"
#include "../../ecs/component/vertex.hpp"
#include "directx_window.hpp"

namespace Spark
{
struct DirectXMesh : public Mesh
{
  public:
    struct IConstantBuffer
    {
        virtual ~IConstantBuffer() = default;
        virtual void update_data(void *new_data) = 0;
        virtual void *get_data_ptr() = 0;
        virtual u64 get_size() const = 0;

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
    };

    template <typename CBufferType> struct ConstantBufferObject : public IConstantBuffer
    {
        explicit ConstantBufferObject(const CBufferType &cb_data) : m_data(cb_data)
        {
            create_buffer(cb_data);
        }

        void create_buffer(const CBufferType &cb_data)
        {
            auto &dx_window = Engine::get<DirectXWindow>();
            auto device = dx_window.get_window_data().m_device;
            u64 buffer_size = sizeof(CBufferType);

            D3D11_BUFFER_DESC buffer_desc = {};
            buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
            buffer_desc.ByteWidth = static_cast<UINT>(buffer_size);
            buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            D3D11_SUBRESOURCE_DATA init_data = {};
            init_data.pSysMem = &cb_data;
        }

        void update_data(void *new_data) override
        {
            m_data = *static_cast<CBufferType *>(new_data);
        }

        void *get_data_ptr() override
        {
            return &m_data;
        }

        u64 get_size() const override
        {
            return sizeof(CBufferType);
        }

        CBufferType m_data;
    };

    template <typename... CBTypes>
    DirectXMesh(const std::vector<Vertex> &vertices, const std::vector<u32> &indices, const CBTypes &...cb_data)
        : Mesh(vertices, indices)
    {
        create();
        (add_cb(cb_data), ...);
    }

    ~DirectXMesh()
    {
        // Resources will be released automatically by ComPtr
    }

    void create()
    {
        create_vertex_buffer();
        if (!m_indices.empty())
        {
            create_index_buffer();
        }
    }

    template <typename... CBTypes>
    void update(const std::vector<Vertex> &vertices, const std::vector<u32> &indices, const CBTypes &...cb_data)
    {
        m_vertices = vertices;
        m_indices = indices;
        (add_cb(cb_data), ...);

        create();
    }

    template <typename CBufferType> void add_cb(const CBufferType &cb_data)
    {
        m_cb_list.emplace_back(std::make_unique<ConstantBufferObject<CBufferType>>(cb_data));
    }

    void update_constant_buffers()
    {
        auto &dx_window = Engine::get<DirectXWindow>();
        auto context = dx_window.get_window_data().m_device_context;

        for (auto &cb : m_cb_list)
        {
            D3D11_MAPPED_SUBRESOURCE mapped_resource;
            std::memcpy(mapped_resource.pData, cb->get_data_ptr(), cb->get_size());
            context->Unmap(cb->m_buffer.Get(), 0);
        }
    }

    void draw()
    {
        auto &dx_window = Engine::get<DirectXWindow>();
        auto context = dx_window.get_window_data().m_device_context;

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, m_vertex_buffer.GetAddressOf(), &stride, &offset);

        if (!m_indices.empty())
        {
            context->IASetIndexBuffer(m_index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(static_cast<UINT>(m_indices.size()), 0, 0);
        }
        else
        {
            context->Draw(static_cast<UINT>(m_vertices.size()), 0);
        }
    }

  public:
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_index_buffer;
    std::vector<std::unique_ptr<IConstantBuffer>> m_cb_list;
  private:
    void create_vertex_buffer()
    {
        auto &dx_window = Engine::get<DirectXWindow>();
        auto device = dx_window.get_window_data().m_device;

        u64 buffer_size = sizeof(m_vertices[0]) * m_vertices.size();

        D3D11_BUFFER_DESC buffer_desc = {};
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.ByteWidth = static_cast<UINT>(buffer_size);
        buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA init_data = {};
        init_data.pSysMem = m_vertices.data();
    }

    void create_index_buffer()
    {
        auto &dx_window = Engine::get<DirectXWindow>();
        auto device = dx_window.get_window_data().m_device;

        u64 buffer_size = sizeof(m_indices[0]) * m_indices.size();

        D3D11_BUFFER_DESC buffer_desc = {};
        buffer_desc.Usage = D3D11_USAGE_DEFAULT;
        buffer_desc.ByteWidth = static_cast<UINT>(buffer_size);
        buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA init_data = {};
        init_data.pSysMem = m_indices.data();
    }
};
} // namespace Spark

#endif // SPARK_DIRECTX_MESH_HPP
