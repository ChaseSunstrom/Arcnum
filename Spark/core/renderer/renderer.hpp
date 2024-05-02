#ifndef SPARK_RENDERER_H
#define SPARK_RENDERER_H

#include "../spark.hpp"

#include "../scene/scene.hpp"
#include "../events/event.hpp"
#include "../ecs/ecs.hpp"
#include "../user/camera.hpp"
#include "../logging/log.hpp"
#include "../util/Singleton.hpp"
#include "instancer.hpp"

namespace spark
{
	struct RendererSettings
	{
		RendererSettings(
				i32 resolution_width = 1920,
				i32 resolution_height = 1080,
				i32 min_width = 800,
				i32 min_height = 600,
				i32 max_width = 3840,
				i32 max_height = 2160,
				i32 texture_quality = 1,
				i32 shadow_quality = 1,
				i32 reflection_quality = 1,
				i32 anti_aliasing_level = 1,
				i32 lighting_quality = 1,
				i32 volumetric_lighting_quality = 0,
				i32 particle_quality = 1,
				f32 view_distance = 1000.0f,
				bool occlusion_culling = true,
				i32 max_frames = 60,
				bool textures = true,
				bool anti_aliasing = true,
				bool shadows = true,
				bool reflections = true,
				bool hdr = true,
				bool bloom = true,
				bool volumetric_lighting = false,
				bool motion_blur = false,
				bool wireframe_mode = false,
				bool show_bounding_boxes = false,
				bool fps_counter = true,
				bool light_visualizer = false,
				i32 tessellation_quality = 0,
				i32 foliage_quality = 1,
				i32 water_quality = 1,
				i32 post_processing_quality = 1,
				i32 global_illumination_quality = 1,
				bool dynamic_resolution = false,
				i32 cpu_thread_usage = 1,
				bool gpu_async_compute = false,
				bool soft_particles = true,
				bool screen_space_reflections = true,
				bool depth_of_field = false,
				bool ambient_occlusion = true,
				bool color_grading = true,
				bool normals_visualizer = false,
				bool uv_debug_mode = false,
				bool physics_debug_drawer = false,
				bool lighting_only_mode = false,
				f32 ui_scale = 1.0f,
				bool subtitles = false,
				i32 text_size = 1) :
				m_resolution_width(resolution_width), m_resolution_height(resolution_height), m_min_width(min_width), m_min_height(
				min_height), m_max_width(max_width), m_max_height(max_height), m_texture_quality(texture_quality), m_shadow_quality(
				shadow_quality), m_reflection_quality(reflection_quality), m_anti_aliasing_level(anti_aliasing_level), m_lighting_quality(
				lighting_quality), m_volumetric_lighting_quality(volumetric_lighting_quality), m_particle_quality(
				particle_quality), m_view_distance(view_distance), m_occlusion_culling(occlusion_culling), m_max_frames(
				max_frames), m_textures(textures), m_anti_aliasing(anti_aliasing), m_shadows(shadows), m_reflections(
				reflections), m_hdr(hdr), m_bloom(bloom), m_volumetric_lighting(volumetric_lighting), m_motion_blur(
				motion_blur), m_wireframe_mode(wireframe_mode), m_show_bounding_boxes(show_bounding_boxes), m_fps_counter(
				fps_counter), m_light_visualizer(light_visualizer), m_tessellation_quality(tessellation_quality), m_foliage_quality(
				foliage_quality), m_water_quality(water_quality), m_post_processing_quality(post_processing_quality), m_global_illumination_quality(
				global_illumination_quality), m_dynamic_resolution(dynamic_resolution), m_soft_particles(soft_particles), m_screen_space_reflections(
				screen_space_reflections), m_depth_of_field(depth_of_field), m_ambient_occlusion(ambient_occlusion), m_color_grading(
				color_grading), m_normals_visualizer(normals_visualizer), m_uv_debug_mode(uv_debug_mode), m_physics_debug_drawer(
				physics_debug_drawer), m_lighting_only_mode(lighting_only_mode), m_ui_scale(ui_scale), m_subtitles(
				subtitles), m_text_size(text_size) { }

		i32 m_resolution_width = 1920;

		i32 m_resolution_height = 1080;

		i32 m_min_width = 800;

		i32 m_min_height = 600;

		i32 m_max_width = 3840;

		i32 m_max_height = 2160;

		// Quality settings
		i32 m_texture_quality = 1; // 0: Low, 1: Medium, 2: High
		i32 m_shadow_quality = 1;  // 0: Low, 1: Medium, 2: High
		i32 m_reflection_quality = 1; // 0: Low, 1: Medium, 2: High
		i32 m_anti_aliasing_level = 1; // 0: Off, 1: FXAA, 2: MSAAx2, 3: MSAAx4, etc.
		i32 m_lighting_quality = 1; // 0: Low, 1: Medium, 2: High
		i32 m_volumetric_lighting_quality = 0; // 0: Off, 1: Low, 2: Medium, 3: High
		i32 m_particle_quality = 1; // 0: Low, 1: Medium, 2: High

		// Performance optimization
		f32 m_view_distance = 1000.0f; // Maximum view distance
		bool m_occlusion_culling = true; // Toggle occlusion culling
		i32 m_max_frames = 60; // Framerate cap

		// Feature toggles
		bool m_textures = true; // Toggle textures
		bool m_anti_aliasing = true; // Toggle anti-aliasing globally
		bool m_shadows = true; // Toggle shadows
		bool m_reflections = true; // Toggle reflections
		bool m_hdr = true; // Toggle HDR rendering
		bool m_bloom = true; // Toggle bloom effects
		bool m_volumetric_lighting = false; // Toggle volumetric lighting/fog
		bool m_motion_blur = false; // Toggle motion blur

		// Debugging and development
		bool m_wireframe_mode = false; // Render in wireframe mode
		bool m_show_bounding_boxes = false; // Show bounding boxes for debugging
		bool m_fps_counter = true; // Display FPS counter
		bool m_light_visualizer = false; // Visualize lights in the scene

		// Advanced Quality settings
		i32 m_tessellation_quality = 0; // 0: Off, 1: Low, 2: Medium, 3: High
		i32 m_foliage_quality = 1; // 0: Low, 1: Medium, 2: High
		i32 m_water_quality = 1; // 0: Low, 1: Medium, 2: High
		i32 m_post_processing_quality = 1; // 0: Low, 1: Medium, 2: High
		i32 m_global_illumination_quality = 1; // 0: Off, 1: Low, 2: Medium, 3: High

		// Additional Performance optimization
		bool m_dynamic_resolution = false; // Adjust resolution dynamically to maintain framerate

		// Additional Feature toggles
		bool m_soft_particles = true; // Toggle soft particles to prevent harsh intersections with geometry
		bool m_screen_space_reflections = true; // Toggle screen space reflections, can be performance-intensive
		bool m_depth_of_field = false; // Toggle depth of field effect
		bool m_ambient_occlusion = true; // Toggle ambient occlusion for more realistic shadows and depth
		bool m_color_grading = true; // Toggle color grading for cinematic looks

		// Extended Debugging and development
		bool m_normals_visualizer = false; // Visualize normal vectors, useful for debugging shading issues
		bool m_uv_debug_mode = false; // Display UV coordinates as colors, to debug texture mappings
		bool m_physics_debug_drawer = false; // Enable debug drawer for physics Engine (collision shapes, etc.)
		bool m_lighting_only_mode = false; // Render lighting information only, ignoring textures and materials

		f32 m_ui_scale = 1.0f; // Adjust the scale of UI elements for accessibility
		bool m_subtitles = false; // Enable subtitles for game dialogues or important sounds
		i32 m_text_size = 1; // 0: Small, 1: Medium, 2: Large, for UI text size customization
	};

	// ==============================================================================
	// RENDERER:     | Used for rendering and storing rendering data
	// ==============================================================================

	class Renderer :
		public Singleton<Renderer>
	{
	public:
		static Renderer& get()
		{
			static Renderer instance;
			return instance;
		}

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

		std::vector<std::unique_ptr<Camera>>& get_cameras()
		{
			return m_cameras;
		}

	private:

		Renderer() :
			Singleton()
		{
			m_cameras.emplace_back(std::make_unique<Camera>());
		}

		~Renderer() = default;

	private:
		std::unique_ptr<RendererSettings> m_settings = std::make_unique<RendererSettings>();

		std::vector<std::unique_ptr<Camera>> m_cameras;
	};
}

#endif //CORE_RENDERER_H