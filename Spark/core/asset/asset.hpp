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
        // General template is intentionally undefined
        template<typename T, typename... Args>
        static void create_asset(const Args&... args);

        template<typename... Args>
        static Mesh& create_asset<Mesh>(const Args&... args)
        {
            auto& mesh_manager = Engine::get<MeshManager>();
            return mesh_manager.create_mesh(args...);
        }

        template<typename... Args>
        static Material& create_asset<Material>(const Args&... args)
        {
            auto& material_manager = Engine::get<MaterialManager>();
            return material_manager.create_material(args...);
        }

        template<typename... Args>
        static Sound& create_asset<Sound>(const Args&... args)
        {
            auto& sound_manager = Engine::get<AudioManager>();
            reeturn sound_manager.create_sound(args...);
        }

        template<typename... Args>
        static Texture& create_asset<Texture>(const Args&... args)
        {
            auto& texture_manager = Engine::get<TextureManager>();
            return texture_manager.create_texture(args...);
        }

        template<typename... Args>
        static void create_asset<Shader>(const Args&... args)
        {
            auto& shader_manager = Engine::get<ShaderManager>();
            shader_manager.load_shader(args...);
        }

        template<typename... Args>
        static UIComponent& create_asset<UIComponent>(const Args&... args)
        {
            auto& ui_manager = Engine::get<UIManager>();
            return ui_manager.create_component(args...);
        }

        template<typename... Args>
        static Scene& create_asset<Scene>(const std::string& name, const Args&... args)
        {
            auto& scene_manager = Engine::get<SceneManager>();
            std::unique_ptr<Scene> scene = std::make_unique<Scene>(SceneConfig(args...));
            return scene_manager.add_scene(name, std::move(scene));
        }
    };

}

#endif