#include "../app/app.hpp"
#include "../ecs/component/shader.hpp"

namespace spark
{
	void renderer::apply_dynamic_resolution() {}

	void renderer::render_with_anti_aliasing() {}

	void renderer::render_shadows() {}

	void renderer::render_reflections() {}

	void renderer::apply_hdr() {}

	void renderer::apply_bloom() {}

	void renderer::render_volumetric_lighting() { }

	void renderer::apply_motion_blur() {}

	void renderer::apply_depth_of_field() {}

	void renderer::apply_ambient_occlusion() {}

	void renderer::apply_color_grading() {}

	void renderer::toggle_wireframe_mode() {}

	void renderer::render_debugging_tools() {}

	void renderer::finalize_frame() {}

	void renderer::render(scene& scene)
	{
		scene_config& config = scene.get_scene_config();
		math::vec4 bc = config.m_background_color;
		set_background_color(bc.r, bc.g, bc.b, bc.a);

		//apply_dynamic_resolution();    // Adjusts resolution based on performance settings
		render_with_anti_aliasing();   // Applies anti-aliasing if enabled
		render_shadows();              // Renders shadows if enabled
		render_reflections();          // Renders reflections if enabled
		apply_hdr();                   // Applies HDR rendering if enabled
		apply_bloom();                 // Applies bloom effects if enabled
		render_volumetric_lighting();  // Renders volumetric lighting if enabled
		apply_motion_blur();           // Applies motion blur if enabled
		apply_depth_of_field();        // Applies depth of field if enabled
		apply_ambient_occlusion();     // Applies ambient occlusion if enabled
		apply_color_grading();         // Applies color grading if enabled
		toggle_wireframe_mode();       // Toggles wireframe mode if enabled for debugging
		render_debugging_tools();      // Renders any additional debugging tools like bounding boxes, normals visualizer, etc.

		m_instancer->render_instanced(m_cameras, scene);
	}
}