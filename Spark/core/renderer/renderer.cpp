#include "../app/app.hpp"
#include "../ecs/component/shader.hpp"

namespace Spark {
void Renderer::apply_dynamic_resolution() {}

void Renderer::render_with_anti_aliasing() {}

void Renderer::render_shadows() {}

void Renderer::render_reflections() {}

void Renderer::apply_hdr() {}

void Renderer::apply_bloom() {}

void Renderer::render_volumetric_lighting() {}

void Renderer::apply_motion_blur() {}

void Renderer::apply_depth_of_field() {}

void Renderer::apply_ambient_occlusion() {}

void Renderer::apply_color_grading() {}

void Renderer::toggle_wireframe_mode() {}

void Renderer::render_debugging_tools() {}

void Renderer::finalize_frame() {}

void Renderer::render() {
  apply_dynamic_resolution();   // Adjusts resolution based on performance
                                // settings
  render_with_anti_aliasing();  // Applies anti-aliasing if enabled
  render_shadows();             // Renders shadows if enabled
  render_reflections();         // Renders reflections if enabled
  apply_hdr();                  // Applies HDR rendering if enabled
  apply_bloom();                // Applies bloom effects if enabled
  render_volumetric_lighting(); // Renders volumetric lighting if enabled
  apply_motion_blur();          // Applies motion blur if enabled
  apply_depth_of_field();       // Applies depth of field if enabled
  apply_ambient_occlusion();    // Applies ambient occlusion if enabled
  apply_color_grading();        // Applies color grading if enabled
  toggle_wireframe_mode();  // Toggles wireframe mode if enabled for debugging
  render_debugging_tools(); // Renders any additional debugging tools like
                            // bounding boxes, normals visualizer, etc.

  Instancer::get().render_instanced(m_cameras,
                                    SceneManager::get().get_current_scene());
}
} // namespace spark