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
template <typename AssetType> struct AssetManagerTraits;

template <> struct AssetManagerTraits<Mesh>
{
    using Manager = MeshManager;
};

template <> struct AssetManagerTraits<Material>
{
    using Manager = MaterialManager;
};

template <> struct AssetManagerTraits<Audio>
{
    using Manager = AudioManager;
};

template <> struct AssetManagerTraits<Texture>
{
    using Manager = TextureManager;
};

template <> struct AssetManagerTraits<Shader>
{
    using Manager = ShaderManager;
};

template <> struct AssetManagerTraits<UIComponent>
{
    using Manager = UIManager;
};

template <> struct AssetManagerTraits<Scene>
{
    using Manager = SceneManager;
};

class AssetManager
{
  public:
    template <typename AssetType, typename... Args> static decltype(auto) create_asset(Args &&...args)
    {
        using ManagerType = typename AssetManagerTraits<AssetType>::Manager;
        auto &manager = Engine::get<ManagerType>();
        return manager.create(std::forward<Args>(args)...);
    }
};
} // namespace Spark

#endif // SPARK_CORE_ASSET_HPP