#ifndef SPARK_TEXTURE_MANAGER_HPP
#define SPARK_TEXTURE_MANAGER_HPP

#include "texture.hpp"

namespace Spark
{

class TextureManager
{
  public:
    static TextureManager &get()
    {
        static TextureManager instance;
        return instance;
    }

    void destroy_texture(const std::string &name)
    {
        m_textures.erase(name);
    }

    Texture &get_texture(const std::string &name)
    {
        return *m_textures[name];
    }

    std::unordered_map<std::string, std::unique_ptr<Texture>> &get_textures()
    {
        return m_textures;
    }

    Texture &create_default_texture()
    {
        static Texture texture;
        return texture;
    }

    Texture &create(const std::string &name, const std::string &path, TextureType type, bool flip)
    {
        // Placeholder for now
        return create_default_texture();
    }

  private:
    TextureManager() = default;

    ~TextureManager() = default;

  private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
};
} // namespace Spark

#endif