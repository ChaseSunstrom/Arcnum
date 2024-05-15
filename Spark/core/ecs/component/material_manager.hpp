#ifndef SPARK_MATERIAL_MANAGER_HPP
#define SPARK_MATERIAL_MANAGER_HPP

#include "texture_manager.hpp"
#include "material.hpp"
#include "shader.hpp"

namespace Spark
{
class MaterialManager
{
  public:
    static MaterialManager &get()
    {
        static MaterialManager instance;
        return instance;
    }

    Material &create(
        const std::string &name, const math::vec4 &color = math::vec4(1), const std::string &texture_name = "", const std::vector<std::string> &shader_paths = {},
        i32 diffuse = 0, i32 specular = 0, i32 ambient = 1, f32 shininess = 0)
    {
        if (m_materials.contains(name))
        {
            return *m_materials[name];
        }

        Texture *tex = nullptr;

        if (!texture_name.empty() && Engine::get<TextureManager>().get_textures().contains(texture_name))
        {
            tex = &(Engine::get<TextureManager>().get_texture(texture_name));
        }
        else
        {
            tex = &(Engine::get<TextureManager>().create_default_texture());
        }

        m_materials[name] = std::make_unique<Material>(color, *tex, shader_paths, diffuse, specular, ambient, shininess);

        return *m_materials[name];
    }

    void load_material(const std::string &name, std::unique_ptr<Material> mat)
    {
        m_materials[name] = std::move(mat);
    }

    Material &get_material(const std::string &name)
    {
        return *m_materials.at(name);
    }

    void destroy_material(const std::string &name)
    {
        m_materials.erase(name);
    }

  private:
    MaterialManager()
    {
        create("__default__");
    }

    ~MaterialManager() = default;

  private:

    std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
};
}

#endif