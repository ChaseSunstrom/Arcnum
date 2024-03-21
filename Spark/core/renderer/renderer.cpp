#include "../app/app.hpp"

namespace spark
{
    void renderer::render(scene& scene)
    {
        clear_screen();

        scene_config& config = scene.get_scene_config();
        math::vec4 bc = config.m_background_color;

        set_background_color(bc.r, bc.g, bc.b, bc.a);

        m_instancer->render_instanced();
    }
}