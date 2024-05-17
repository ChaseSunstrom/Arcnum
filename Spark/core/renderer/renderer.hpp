#ifndef SPARK_RENDERER_H
#define SPARK_RENDERER_H

#include "../spark.hpp"

#include "../ecs/ecs.hpp"
#include "../events/event.hpp"
#include "../logging/log.hpp"
#include "../scene/scene.hpp"
#include "../user/camera.hpp"
#include "../util/Singleton.hpp"
#include "instancer.hpp"

namespace Spark
{
struct RendererSettings
{
    i32 m_resolution_width = 1920;

    i32 m_resolution_height = 1080;

    i32 m_min_width = 800;

    i32 m_min_height = 600;

    i32 m_max_width = 3840;

    i32 m_max_height = 2160;

    // Quality settings
    i32 m_texture_quality = 1;             // 0: Low, 1: Medium, 2: High
    i32 m_shadow_quality = 1;              // 0: Low, 1: Medium, 2: High
    i32 m_reflection_quality = 1;          // 0: Low, 1: Medium, 2: High
    i32 m_anti_aliasing_level = 1;         // 0: Off, 1: FXAA, 2: MSAAx2, 3: MSAAx4, etc.
    i32 m_lighting_quality = 1;            // 0: Low, 1: Medium, 2: High
    i32 m_volumetric_lighting_quality = 0; // 0: Off, 1: Low, 2: Medium, 3: High
    i32 m_particle_quality = 1;            // 0: Low, 1: Medium, 2: High

    // Performance optimization
    f32 m_view_distance = 1000.0f;   // Maximum view distance
    bool m_occlusion_culling = true; // Toggle occlusion culling
    i32 m_max_frames = 60;           // Framerate cap

    // Feature toggles
    bool m_textures = true;             // Toggle textures
    bool m_anti_aliasing = true;        // Toggle anti-aliasing globally
    bool m_shadows = true;              // Toggle shadows
    bool m_reflections = true;          // Toggle reflections
    bool m_hdr = true;                  // Toggle HDR rendering
    bool m_bloom = true;                // Toggle bloom effects
    bool m_volumetric_lighting = false; // Toggle volumetric lighting/fog
    bool m_motion_blur = false;         // Toggle motion blur

    // Debugging and development
    bool m_wireframe_mode = false;      // Render in wireframe mode
    bool m_show_bounding_boxes = false; // Show bounding boxes for debugging
    bool m_fps_counter = true;          // Display FPS counter
    bool m_light_visualizer = false;    // Visualize lights in the scene

    // Advanced Quality settings
    i32 m_tessellation_quality = 0;        // 0: Off, 1: Low, 2: Medium, 3: High
    i32 m_foliage_quality = 1;             // 0: Low, 1: Medium, 2: High
    i32 m_water_quality = 1;               // 0: Low, 1: Medium, 2: High
    i32 m_post_processing_quality = 1;     // 0: Low, 1: Medium, 2: High
    i32 m_global_illumination_quality = 1; // 0: Off, 1: Low, 2: Medium, 3: High

    // Additional Performance optimization
    bool m_dynamic_resolution = false; // Adjust resolution dynamically to maintain framerate

    // Additional Feature toggles
    bool m_soft_particles = true;           // Toggle soft particles to prevent harsh
                                            // intersections with geometry
    bool m_screen_space_reflections = true; // Toggle screen space reflections, can be performance-intensive
    bool m_depth_of_field = false;          // Toggle depth of field effect
    bool m_ambient_occlusion = true;        // Toggle ambient occlusion for more realistic shadows and depth
    bool m_color_grading = true;            // Toggle color grading for cinematic looks

    // Extended Debugging and development
    bool m_normals_visualizer = false;   // Visualize normal vectors, useful for debugging shading issues
    bool m_uv_debug_mode = false;        // Display UV coordinates as colors, to debug texture mappings
    bool m_physics_debug_drawer = false; // Enable debug drawer for physics Engine (collision shapes, etc.)
    bool m_lighting_only_mode = false;   // Render lighting information only,
                                         // ignoring textures and materials

    f32 m_ui_scale = 1.0f;    // Adjust the scale of UI elements for accessibility
    bool m_subtitles = false; // Enable subtitles for game dialogues or important sounds
    i32 m_text_size = 1;      // 0: Small, 1: Medium, 2: Large, for UI text size customization
};

class Renderer : public Singleton<Renderer>
{
    friend class Singleton<Renderer>;
  public:
    void render();

    void apply_dynamic_resolution();

    void render_with_anti_aliasing();

    void render_shadows();

    void render_reflections();

    void apply_hdr();

    void apply_bloom();

    void render_volumetric_lighting();

    void apply_motion_blur();

    void apply_depth_of_field();

    void apply_ambient_occlusion();

    void apply_color_grading();

    void toggle_wireframe_mode();

    void render_debugging_tools();

    void finalize_frame();

  private:
    Renderer() = default;

    ~Renderer() = default;

  private:
    std::unique_ptr<RendererSettings> m_settings = std::make_unique<RendererSettings>();
};
} // namespace Spark

#endif // CORE_RENDERER_H