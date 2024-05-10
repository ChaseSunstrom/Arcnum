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

    void destroy_texture(const std::string &name);

    Texture &get_texture(const std::string &name);

    Texture &create(const std::string &name, const std::string &path, TextureType type = TextureType::TWO_D,
                    bool flip = true);

    Texture &create_default_texture();

    std::unordered_map<std::string, std::unique_ptr<Texture>> &get_textures();

  private:
    TextureManager() = default;

    ~TextureManager() = default;

  private:
    std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
};
}

#endif