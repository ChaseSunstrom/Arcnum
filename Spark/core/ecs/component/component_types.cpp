#include "component_types.hpp"
#include "../../window/window_manager.hpp"
// Because stb_image is fucking weird
#define STB_IMAGE_IMPLEMENTATION

#include <OTHER/stb_image.h>

namespace Spark
{

void TextureManager::destroy_texture(const std::string &name)
{
    m_textures.erase(name);
}

Texture &TextureManager::get_texture(const std::string &name)
{
    return *m_textures[name];
}

std::unordered_map<std::string, std::unique_ptr<Texture>> &TextureManager::get_textures()
{
    return m_textures;
}

Texture &TextureManager::create_default_texture()
{
    static Texture texture;
    return texture;
}

Texture &TextureManager::create(const std::string &name, const std::string &path, TextureType type, bool flip)
{
    // Placeholder for now
    return create_default_texture();
}

} // namespace Spark