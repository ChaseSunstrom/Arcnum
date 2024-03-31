#include "../app/app.hpp"
#include "../ecs/component/shader.hpp"

namespace spark
{
	void renderer::apply_dynamic_resolution()
	{
		auto& window_ref = engine::get<window>();
		auto& window_data_ref = window_ref.get_window_data();

		static auto last_time = std::chrono::high_resolution_clock::now();
		auto current_time = std::chrono::high_resolution_clock::now();
		float delta_time = std::chrono::duration<float, std::chrono::seconds::period>(current_time - last_time).count();
		last_time = current_time;

		float target_frame_time = 1.0f / m_settings->m_max_frames; // Target frame time to achieve target FPS
		float scale_adjustment_factor = 1.05f; // Adjust based on how aggressively you want to scale

		// If frame took longer than target, reduce resolution
		if (delta_time > target_frame_time)
		{
			m_settings->m_resolution_width /= scale_adjustment_factor;
			m_settings->m_resolution_height /= scale_adjustment_factor;
		}
		else if (delta_time < target_frame_time * 0.85f) // Arbitrary threshold to start increasing resolution again
		{
			m_settings->m_resolution_width *= scale_adjustment_factor;
			m_settings->m_resolution_height *= scale_adjustment_factor;
		}

		// Ensure we don't go below a minimum resolution or above the maximum resolution
		m_settings->m_resolution_width = std::max(m_settings->m_min_width, std::min(m_settings->m_resolution_width, window_data_ref.m_width));
		m_settings->m_resolution_height = std::max(m_settings->m_min_height, std::min(m_settings->m_resolution_height, window_data_ref.m_height));

		// Resize framebuffer texture
		glBindTexture(GL_TEXTURE_2D, window_data_ref.m_texture_color_buffer);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_settings->m_resolution_width, m_settings->m_resolution_height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		glBindFramebuffer(GL_FRAMEBUFFER, window_data_ref.m_framebuffer);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, window_data_ref.m_texture_color_buffer, 0);

		// Check the framebuffer status
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			// Handle incomplete framebuffer status
			SPARK_ERROR("Framebuffer not complete after resizing");
		}
		// Update the viewport in case the resolution has changed
		glViewport(0, 0, m_settings->m_resolution_width, m_settings->m_resolution_height);

		// Update projection matrix and other necessary states if needed
	}


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

	void render_node(const octree& node, const math::mat4& viewMatrix, const math::mat4& projectionMatrix, GLuint shaderProgram)
	{
		std::vector<math::vec3> vertices;
		node.get_node_edges(vertices); // Get the edges of the current node

		// Bind and buffer data for the current node
		GLuint vao, vbo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(math::vec3), vertices.data(), GL_STATIC_DRAW);

		// Set up the vertex attributes (position)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(math::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		// Render the lines
		glDrawArrays(GL_LINES, 0, vertices.size());

		// Clean up
		glBindVertexArray(0);
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);

		// Recursively render children if this is not a leaf node
		if (!node.m_is_leaf)
		{
			for (const auto& child : node.m_children)
			{
				if (child)
				{
					render_node(*child, viewMatrix, projectionMatrix, shaderProgram); // Recursively render the child
				}
			}
		}
	}

	void renderer::render_octree(camera& camera, octree& root_node, math::mat4& modelMatrix)
	{
		shader_manager& shader_man = engine::get<shader_manager>();
		GLuint shaderProgram = shader_man.get_shader("Spark/shaders/line.vert", "Spark/shaders/line.frag");

		// Use shader and set uniforms
		glUseProgram(shaderProgram);

		math::mat4 viewMatrix = camera.get_view_matrix();
		math::mat4 projectionMatrix = camera.get_projection_matrix();

		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		GLint lineColorLoc = glGetUniformLocation(shaderProgram, "lineColor");

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, math::value_ptr(modelMatrix));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, math::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, math::value_ptr(projectionMatrix));
		glUniform3f(lineColorLoc, 0.0f, 0.0f, 1.0f); // Green lines

		// Start the recursive rendering process from the root node
		render_node(root_node, viewMatrix, projectionMatrix, shaderProgram);

		// Clean up
		glUseProgram(0);
	}

	void renderer::render_frustum(camera& camera, frustum& frustum)
	{
		static GLuint vao = 0;
		static GLuint vbo = 0;

		if (vao == 0 && vbo == 0)
		{
			// Only initialize once
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vbo);
		}

		shader_manager& shader_man = engine::get<shader_manager>();
		GLuint shaderProgram = shader_man.get_shader("Spark/shaders/line.vert", "Spark/shaders/line.frag");

		// Bind shader and set uniforms
		glUseProgram(shaderProgram);

		math::mat4 viewMatrix = camera.get_view_matrix();
		math::mat4 projectionMatrix = camera.get_projection_matrix();

		GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
		GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
		GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
		GLint lineColorLoc = glGetUniformLocation(shaderProgram, "lineColor");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, math::value_ptr(math::mat4(1.0f))); // Model matrix is identity
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, math::value_ptr(viewMatrix));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, math::value_ptr(projectionMatrix));
		glUniform3f(lineColorLoc, 0.0f, 1.0f, 0.0f);

		glBindVertexArray(vao);

		std::vector<math::vec3> vertices;
		auto corners = frustum.get_corners();

		// Define indices for the frustum corners in the order of near plane to far plane
		const int indices[] = {
			0, 1, 1, 2, 2, 3, 3, 0, // near plane
			4, 5, 5, 6, 6, 7, 7, 4, // far plane
			0, 4, 1, 5, 2, 6, 3, 7  // sides
		};

		// Create lines between the corner points using the indices
		for (int i = 0; i < 24; i += 2)
		{
			vertices.push_back(corners[indices[i]]);
			vertices.push_back(corners[indices[i + 1]]);
		}


		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(math::vec3), vertices.data(), GL_DYNAMIC_DRAW);

		// Set up the vertex attributes (position)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(math::vec3), (void*)0);
		glEnableVertexAttribArray(0);

		// Draw the frustum lines
		glDrawArrays(GL_LINES, 0, vertices.size());

		// If the frustum is dynamic and changes every frame, you can delete VAO and VBO here
		// Otherwise, keep them for performance and delete them at the end of the program or when the frustum is no longer needed

		glBindVertexArray(0);
		glUseProgram(0);
	}

	void renderer::render(scene& scene)
	{
		scene_config& config = scene.get_scene_config();
		math::vec4 bc = config.m_background_color;
		set_background_color(bc.r, bc.g, bc.b, bc.a);

		apply_dynamic_resolution();    // Adjusts resolution based on performance settings
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
#define RENDERER_DEBUG
#ifdef RENDERER_DEBUG
		render_frustum(*m_cameras[0], *m_cameras[0]->m_frustum);
		math::mat4 modelMatrix = math::mat4(1.0f);
		render_octree(*m_cameras[0], scene.get_octree(), modelMatrix);
#endif
	}
}