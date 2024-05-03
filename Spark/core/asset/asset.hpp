#ifndef SPARK_CORE_ASSET_HPP
#define SPARK_CORE_ASSET_HPP

#include "../spark.hpp"
#include "../ecs/component/component_types.hpp"
#include "../ecs/component/shader.hpp"
#include "../audio/audio.hpp"
#include "../ui/ui.hpp"
#include "../scene/scene_manager.hpp"

namespace spark
{
    class AssetManager
    {
    public:
        template<typename T, typename... Args>
        static T& create_asset(Args&&... args);
    };

    template<typename... Args>
    Mesh& AssetManager::create_asset<Mesh>(Args&&... args)
    {
        auto& mesh_manager = Engine::get<MeshManager>();
        return mesh_manager.create_mesh(std::forward<Args>(args)...);
    }

    template<typename... Args>
    Material& AssetManager::create_asset<Material>(Args&&... args)
    {
        auto& material_manager = Engine::get<MaterialManager>();
        return material_manager.create_material(std::forward<Args>(args)...);
    }

    template<typename... Args>
    Sound& AssetManager::create_asset<Sound>(Args&&... args)
    {
        auto& sound_manager = Engine::get<AudioManager>();
        return sound_manager.create_sound(std::forward<Args>(args)...);
    }

    template<typename... Args>
    Texture& AssetManager::create_asset<Texture>(Args&&... args)
    {
        auto& texture_manager = Engine::get<TextureManager>();
        return texture_manager.create_texture(std::forward<Args>(args)...);
    }

    template<typename... Args>
    Shader& AssetManager::create_asset<Shader>(Args&&... args)
    {
        auto& shader_manager = Engine::get<ShaderManager>();
        return shader_manager.load_shader(std::forward<Args>(args)...);
    }

    template<typename... Args>
    UIComponent& AssetManager::create_asset<UIComponent>(Args&&... args)
    {
        auto& ui_manager = Engine::get<UIManager>();
        return ui_manager.create_component(std::forward<Args>(args)...);
    }

    template<typename... Args>
    Scene& AssetManager::create_asset<Scene>(const std::string& name, Args&&... args)
    {
        auto& scene_manager = Engine::get<SceneManager>();
        std::unique_ptr<Scene> scene = std::make_unique<Scene>(SceneConfig(std::forward<Args>(args)...));
        return scene_manager.add_scene(name, std::move(scene));
    }
}

#endif  // SPARK_CORE_ASSET_HPP