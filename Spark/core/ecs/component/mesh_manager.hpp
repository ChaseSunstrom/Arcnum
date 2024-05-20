#ifndef SPARK_MESH_MANAGER_HPP
#define SPARK_MESH_MANAGER_HPP

#include "../../spark.hpp"

#include "mesh.hpp"
#include "../../window/vulkan/vulkan_mesh.hpp"

namespace Spark
{

class MeshManager
{
  public:
    static MeshManager &get()
    {
        static MeshManager instance;
        return instance;
    }

    template <typename... Args>
    Mesh &create(const std::string &name, const std::vector<Vertex> &vertices, const std::vector<u32> indices = {},
                 Args &&...args)
    {
        if (m_meshes.contains(name))
        {
            return *m_meshes[name];
        }

        if (is_current_api(API::VULKAN))
        {
            m_meshes[name] = std::make_unique<VulkanMesh>(vertices, indices, std::forward<Args>(args)...);
            return *m_meshes[name];
        }
    }

    void load_mesh(const std::string &name, std::unique_ptr<Mesh> mesh)
    {
        m_meshes[name] = std::move(mesh);
    }

    Mesh &get_mesh(const std::string &name)
    {
        return *m_meshes[name];
    }

    void destroy_mesh(const std::string &name)
    {
        m_meshes.erase(name);
    }

  private:
    MeshManager() = default;

    ~MeshManager() = default;

  private:
    std::unordered_map<std::string, std::unique_ptr<Mesh>> m_meshes;
};
}

#endif