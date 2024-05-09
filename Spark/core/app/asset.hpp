#ifndef SPARK_CORE_ASSET_HPP
#define SPARK_CORE_ASSET_HPP

#include "../audio/audio.hpp"
#include "../ecs/component/component_types.hpp"
#include "../ecs/component/shader.hpp"
#include "../scene/scene_manager.hpp"
#include "../spark.hpp"
#include "../ui/ui.hpp"

namespace Spark
{
enum class AssetType
{
    MESH,
    MATERIAL,
    SOUND,
    TEXTURE,
    SHADER,
    UI_ELEMENT,
    SCENE
};

class AssetManager
{
  public:
    template <AssetType T, typename... Args> static void create_asset(const Args &...args)
    {
        switch (T)
        {
        case AssetType::MESH: {
            auto &mesh_manager = Engine::get<MeshManager>();
            return mesh_manager.create(args...);
        }

        case AssetType::MATERIAL: {
            auto &material_manager = Engine::get<MaterialManager>();
            return material_manager.create(args...);
        }

        case AssetType::SOUND: {
            auto &sound_manager = Engine::get<AudioManager>();
            return sound_manager.create_sound(args...);
        }

        case AssetType::TEXTURE: {
            auto &texture_manager = Engine::get<TextureManager>();
            return texture_manager.create(args...);
        }

        case AssetType::SHADER: {
            auto &shader_manager = Engine::get<ShaderManager>();
            return shader_manager.load_shader(args...);
        }

        case AssetType::UI_ELEMENT: {
            auto &ui_manager = Engine::get<UIManager>();
            return ui_manager.create(args...);
        }

        case AssetType::SCENE: {
            auto &scene_manager = Engine::get<SceneManager>();
            return scene_manager.create(args...);
        }
        }
    }
};
} // namespace Spark

#endif