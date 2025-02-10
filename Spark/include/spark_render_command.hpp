#ifndef SPARK_RENDER_COMMAND_HPP
#define SPARK_RENDER_COMMAND_HPP

#include "spark_command.hpp"
#include "spark_shader.hpp"
#include "spark_mesh.hpp"
#include "spark_math.hpp"
#include "spark_renderer.hpp"
#include "spark_camera.hpp" // For Camera, if desired

namespace spark
{
    // A generic render command that does not hardcode any uniform names.
    // The user supplies callbacks to set both regular and instance data.
    // Optionally, a Camera pointer may be attached for convenience, but nothing is set automatically.
    class SPARK_API RenderCommand : public ICommand
    {
    public:
        // A custom command function may override the default behavior.
        using CommandFunction = std::function<void(IRenderer&)>;

        RenderCommand() = default;
        RenderCommand(CommandFunction command_fn)
            : m_command_fn(command_fn)
        {
        }

        // Pointers to the shader and mesh.
        IShaderProgram* shader_program = nullptr;
        IMesh* mesh = nullptr;
        // If non-empty, instanced drawing is used.
        std::vector<math::Mat4> instance_transforms;
        // Optional lambda: set any non-instance-specific uniforms.
        // For example, you can set model, view, projection, camera data, etc.
        std::function<void(IShaderProgram&)> set_uniforms_fn;
        // Optional lambda: set the instance data uniform(s) as desired.
        // For example, you might set a uniform array named "u_instanceData" here.
        std::function<void(IShaderProgram&, const std::vector<math::Mat4>&)> set_instance_uniforms_fn;
        // Optional draw mode (e.g., GL_TRIANGLES, GL_LINES, etc.). Not used by default.
        i32 draw_mode = 0;
        // Optionally attach a Camera. This pointer is available for your use in the uniform callback;
        // nothing is automatically set.
        Camera* camera = nullptr;

        // Execute the command.
        void Execute(IRenderer& renderer)
        {
            if (m_command_fn)
            {
                m_command_fn(renderer);
                return;
            }
            if (shader_program && mesh)
            {
                shader_program->Bind();

                // The user is responsible for setting all uniforms (including any camera data).
                if (set_uniforms_fn)
                {
                    set_uniforms_fn(*shader_program);
                }

                // If instance data is provided, call the lambda to set it, then perform instanced drawing.
                if (!instance_transforms.empty())
                {
                    if (set_instance_uniforms_fn)
                    {
                        set_instance_uniforms_fn(*shader_program, instance_transforms);
                    }
                    else
                    {
                        mesh->SetInstanceData(instance_transforms);
                    }
                    mesh->DrawInstanced(instance_transforms.size());
                }
                else
                {
                    mesh->Draw();
                }

                shader_program->Unbind();
            }
        }

        // ICommand interface implementation.
        void Execute(const std::function<void(ICommand&)>& fn) override
        {
            fn(*this);
        }

    private:
        CommandFunction m_command_fn;
    };
}

#endif // SPARK_RENDER_COMMAND_HPP
