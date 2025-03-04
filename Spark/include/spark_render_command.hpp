#ifndef SPARK_RENDER_COMMAND_HPP
#define SPARK_RENDER_COMMAND_HPP

#include "spark_command.hpp"
#include "spark_shader.hpp"
#include "spark_mesh.hpp"
#include "spark_math.hpp"
#include "spark_renderer.hpp"

namespace spark
{
    // A generic render command that can handle any type of rendering
    class SPARK_API RenderCommand : public ICommand
    {
    public:
        // Custom command function for complete flexibility
        using CommandFunction = std::function<void(IRenderer&)>;
        // Uniform setter function type
        using UniformFunction = std::function<void(IShaderProgram&)>;
        // Instance data setter function type
        using InstanceFunction = std::function<void(IShaderProgram&, const std::vector<math::Mat4>&)>;

        RenderCommand() = default;
        
        // Constructor for custom rendering logic
        explicit RenderCommand(CommandFunction command_fn)
            : m_command_fn(std::move(command_fn))
        {
        }

        // Core rendering components
        IShaderProgram* shader_program = nullptr;
        IMesh* mesh = nullptr;

        // Rendering state
        i32 draw_mode = 0;
        bool depth_test = true;
        bool blending = false;
        bool wireframe = false;

        // Instance data for batched rendering
        std::vector<math::Mat4> instance_transforms;

        // Flexible uniform setting
        UniformFunction set_uniforms_fn;
        InstanceFunction set_instance_uniforms_fn;

        // Optional user data for custom rendering
        std::any user_data;

        // Execute the render command
        void ExecuteRender(IRenderer& renderer) const
        {
            // If custom command function exists, use it
            if (m_command_fn)
            {
                m_command_fn(renderer);
                return;
            }

            // Standard rendering path
            if (!shader_program || !mesh) return;

            shader_program->Bind();

            // Set any uniforms
            if (set_uniforms_fn)
            {
                set_uniforms_fn(*shader_program);
            }

            // Handle instanced rendering
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

        // ICommand interface implementation
        void Execute(const std::function<void(ICommand&)>& fn) override
        {
            fn(*this);
        }

        // Helper methods for easy command creation
        static RenderCommand Create(IMesh* mesh, IShaderProgram* shader)
        {
            RenderCommand cmd;
            cmd.mesh = mesh;
            cmd.shader_program = shader;
            return cmd;
        }

        static RenderCommand CreateInstanced(
            IMesh* mesh, 
            IShaderProgram* shader,
            std::vector<math::Mat4> instances)
        {
            RenderCommand cmd;
            cmd.mesh = mesh;
            cmd.shader_program = shader;
            cmd.instance_transforms = std::move(instances);
            return cmd;
        }

        static RenderCommand CreateCustom(CommandFunction fn)
        {
            return RenderCommand(std::move(fn));
        }

    private:
        CommandFunction m_command_fn;
    };
}

#endif // SPARK_RENDER_COMMAND_HPP
