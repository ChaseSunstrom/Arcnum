#include "../app/app.hpp"
#include "../ecs/component/shader.hpp"

namespace spark
{
    void renderer::render_octree(camera& camera, octree& octree, math::mat4& modelMatrix)
    {
        static GLuint vao = 0;
        static GLuint vbo = 0;

        if (vao == 0 && vbo == 0)
        {
            // Only initialize once
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vbo);
        }

        shader_manager& shader_man = application::get_shader_manager();
        GLuint shaderProgram = shader_man.get_shader("C:\\Users\\Chase\\source\\repos\\Arcnum\\Spark\\shaders\\line.vertC:\\Users\\Chase\\source\\repos\\Arcnum\\Spark\\shaders\\line.frag");
        // Bind shader and set uniforms
        glUseProgram(shaderProgram);

        glm::mat4 viewMatrix = camera.get_view_matrix();
        glm::mat4 projectionMatrix = camera.get_projection_matrix();

        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        GLint lineColorLoc = glGetUniformLocation(shaderProgram, "lineColor");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniform3f(lineColorLoc, 0.0f, 0.0f, 1.0f); // Green lines

        glBindVertexArray(vao);

        std::vector<glm::vec3> vertices;
        octree.get_node_edges(vertices); // Assume this method gives us the edges of the octree

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

        // Set up the vertex attributes (position)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
        glEnableVertexAttribArray(0);

        glDrawArrays(GL_LINES, 0, vertices.size()); // Render the lines

        // If octree is dynamic and changes every frame, you can delete VAO and VBO here
        // Otherwise, keep them for performance and delete them at the end of the program or when the octree is no longer needed

        glBindVertexArray(0);
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

        shader_manager& shader_man = application::get_shader_manager();
        GLuint shaderProgram = shader_man.get_shader("C:\\Users\\Chase\\source\\repos\\Arcnum\\Spark\\shaders\\line.vertC:\\Users\\Chase\\source\\repos\\Arcnum\\Spark\\shaders\\line.frag");

        // Bind shader and set uniforms
        glUseProgram(shaderProgram);

        glm::mat4 viewMatrix = camera.get_view_matrix();
        glm::mat4 projectionMatrix = camera.get_projection_matrix();

        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
        GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
        GLint lineColorLoc = glGetUniformLocation(shaderProgram, "lineColor");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f))); // Model matrix is identity
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        glUniform3f(lineColorLoc, 0.0f, 1.0f, 0.0f); // Red lines

        glBindVertexArray(vao);

        std::vector<glm::vec3> vertices;
        auto corners = frustum.get_corners();

        // Create lines between the corner points
        for (size_t i = 0; i < corners.size(); ++i)
        {
            for (size_t j = i + 1; j < corners.size(); ++j)
            {
                vertices.push_back(corners[i]);
                vertices.push_back(corners[j]);
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_DYNAMIC_DRAW);

        // Set up the vertex attributes (position)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
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
        clear_screen();

        scene_config& config = scene.get_scene_config();
        math::vec4 bc = config.m_background_color;

        set_background_color(bc.r, bc.g, bc.b, bc.a);
        m_instancer->render_instanced(m_cameras, scene);
    #define RENDERER_DEBUG
    #ifdef RENDERER_DEBUG
        render_frustum(*m_cameras[0], *m_cameras[0]->m_frustum);
        math::mat4 modelMatrix = math::mat4(1.0f);
		render_octree(*m_cameras[0], scene.get_octree(), modelMatrix);
	#endif
    }
}