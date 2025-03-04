#include "spark_pch.hpp"
#include "opengl/spark_gl_renderer.hpp"
#include "spark_render_command.hpp"
#include <GL/glew.h>

namespace spark::opengl
{
    namespace
    {
        void CheckGLError(const char* function)
        {
            GLenum error = glGetError();
            if (error != GL_NO_ERROR)
            {
                const char* error_str;
                switch (error)
                {
                    case GL_INVALID_ENUM: error_str = "GL_INVALID_ENUM"; break;
                    case GL_INVALID_VALUE: error_str = "GL_INVALID_VALUE"; break;
                    case GL_INVALID_OPERATION: error_str = "GL_INVALID_OPERATION"; break;
                    case GL_OUT_OF_MEMORY: error_str = "GL_OUT_OF_MEMORY"; break;
                    default: error_str = "Unknown Error"; break;
                }
                // TODO: Use proper logging system
                fprintf(stderr, "OpenGL Error in %s: %s\n", function, error_str);
            }
        }

        // Track current OpenGL state to avoid redundant state changes
        struct GLState
        {
            bool depth_test = true;
            bool blending = false;
            bool wireframe = false;
            GLuint current_shader = 0;
            GLuint current_vao = 0;
            GLuint current_vbo = 0;
            GLuint current_ibo = 0;
        };

        static GLState s_current_state;

        void ApplyRenderState(const RenderCommand& command)
        {
            // Depth testing
            if (command.depth_test != s_current_state.depth_test)
            {
                if (command.depth_test)
                {
                    glEnable(GL_DEPTH_TEST);
                    glDepthFunc(GL_LESS);
                }
                else
                {
                    glDisable(GL_DEPTH_TEST);
                }
                s_current_state.depth_test = command.depth_test;
            }

            // Blending
            if (command.blending != s_current_state.blending)
            {
                if (command.blending)
                {
                    glEnable(GL_BLEND);
                    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                }
                else
                {
                    glDisable(GL_BLEND);
                }
                s_current_state.blending = command.blending;
            }

            // Wireframe mode
            if (command.wireframe != s_current_state.wireframe)
            {
                glPolygonMode(GL_FRONT_AND_BACK, command.wireframe ? GL_LINE : GL_FILL);
                s_current_state.wireframe = command.wireframe;
            }

            CheckGLError("ApplyRenderState");
        }
    }

    void GLRenderer::Initialize()
    {
        // Initialize GLEW

        // Set up default OpenGL state
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Clear color
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        // Initialize state tracking
        s_current_state = GLState{};

        CheckGLError("Initialize");
    }

    void GLRenderer::BeginFrame()
    {
        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Reset viewport to window size
        glViewport(0, 0, m_viewport_width, m_viewport_height);

        // Reset state tracking for new frame
        s_current_state = GLState{};

        CheckGLError("BeginFrame");
    }

    void GLRenderer::EndFrame()
    {
        // Restore default state
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        // Unbind any bound resources
        glUseProgram(0);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        s_current_state = GLState{};

        CheckGLError("EndFrame");
    }

    void GLRenderer::RunRenderCommand(const RenderCommand& command)
    {
        if (!command.mesh || !command.shader_program)
            return;

        // Apply render state
        ApplyRenderState(command);
        
        // Bind shader if different from current
        GLuint shader_id = static_cast<GLuint>(reinterpret_cast<uintptr_t>(command.shader_program));
        if (shader_id != s_current_state.current_shader)
        {
            glUseProgram(shader_id);
            s_current_state.current_shader = shader_id;
            CheckGLError("Shader Bind");
        }

        // Set uniforms
        if (command.set_uniforms_fn)
        {
            command.set_uniforms_fn(*command.shader_program);
            CheckGLError("Set Uniforms");
        }

        // Handle instanced rendering
        if (!command.instance_transforms.empty())
        {
            // Set instance uniforms if provided
            if (command.set_instance_uniforms_fn)
            {
                command.set_instance_uniforms_fn(*command.shader_program, command.instance_transforms);
                CheckGLError("Set Instance Uniforms");
            }

            // Draw instanced
            command.mesh->DrawInstanced(command.instance_transforms.size());
        }
        else
        {
            // Regular draw
            command.mesh->Draw();
        }

        CheckGLError("RunRenderCommand");
    }

    void GLRenderer::DrawSomething()
    {
        // Example implementation
        CheckGLError("DrawSomething");
    }

    void GLRenderer::Shutdown()
    {
        // Reset state tracking
        s_current_state = GLState{};
    }

    void GLRenderer::SetViewport(uint32_t width, uint32_t height)
    {
        m_viewport_width = width;
        m_viewport_height = height;
        glViewport(0, 0, width, height);
        CheckGLError("SetViewport");
    }
}
