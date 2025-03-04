#ifndef SPARK_RENDER_SYSTEM_HPP
#define SPARK_RENDER_SYSTEM_HPP

#include "spark_pch.hpp"
#include "spark_ecs.hpp"
#include "spark_render_components.hpp"
#include "spark_render_command.hpp"
#include "spark_renderer.hpp"
#include "spark_command_queue.hpp"

namespace spark
{
    class RenderSystem
    {
    public:
        explicit RenderSystem(Coordinator& coordinator, CommandQueue& command_queue)
            : m_coordinator(coordinator)
            , m_command_queue(command_queue)
        {
        }

        void Update(Query<RenderableComponent> query)
        {
            // Group renderables by their properties for efficient batching
            struct BatchKey {
                IMesh* mesh;
                IShaderProgram* shader;
                std::shared_ptr<Material> material;
                i32 draw_mode;
                bool depth_test;
                bool blending;
                bool wireframe;

                bool operator==(const BatchKey& other) const {
                    return mesh == other.mesh &&
                           shader == other.shader &&
                           material == other.material &&
                           draw_mode == other.draw_mode &&
                           depth_test == other.depth_test &&
                           blending == other.blending &&
                           wireframe == other.wireframe;
                }
            };

            struct BatchKeyHash {
                size_t operator()(const BatchKey& key) const {
                    size_t h1 = std::hash<void*>{}(static_cast<void*>(key.mesh));
                    size_t h2 = std::hash<void*>{}(static_cast<void*>(key.shader));
                    size_t h3 = std::hash<void*>{}(static_cast<void*>(key.material.get()));
                    size_t h4 = std::hash<int32_t>{}(key.draw_mode);
                    size_t h5 = std::hash<bool>{}(key.depth_test);
                    size_t h6 = std::hash<bool>{}(key.blending);
                    size_t h7 = std::hash<bool>{}(key.wireframe);

                    return ((((((h1 ^ (h2 << 1)) ^ (h3 << 2)) ^ (h4 << 3)) ^ (h5 << 4)) ^ (h6 << 5)) ^ (h7 << 6));
                }
            };

            std::unordered_map<BatchKey, std::vector<RenderableComponent*>, BatchKeyHash> batches;

            // First pass: group renderables
            query.ForEach([&](Entity entity, RenderableComponent& renderable) {
                if (!renderable.visible || !renderable.mesh || !renderable.shader)
                    return;

                BatchKey key{
                    renderable.mesh,
                    renderable.shader,
                    renderable.material,
                    renderable.draw_mode,
                    renderable.depth_test,
                    renderable.blending,
                    renderable.wireframe
                };

                batches[key].push_back(&renderable);
            });

            // Second pass: create and submit render commands
            for (const auto& [key, group] : batches)
            {
                if (group.empty()) continue;

                RenderCommand cmd;
                cmd.mesh = key.mesh;
                cmd.shader_program = key.shader;
                cmd.draw_mode = key.draw_mode;
                cmd.depth_test = key.depth_test;
                cmd.blending = key.blending;
                cmd.wireframe = key.wireframe;

                // If any renderable in the group is instanced, combine instance data
                bool has_instances = false;
                std::vector<math::Mat4> combined_instances;
                for (const auto* renderable : group)
                {
                    if (renderable->is_instanced())
                    {
                        has_instances = true;
                        combined_instances.insert(
                            combined_instances.end(),
                            renderable->instance_transforms.begin(),
                            renderable->instance_transforms.end()
                        );
                    }
                }

                if (has_instances)
                {
                    cmd.instance_transforms = std::move(combined_instances);
                }

                // Set uniforms
                cmd.set_uniforms_fn = [material = key.material](IShaderProgram& shader) {
                    if (material) {
                        material->ApplyToShader(&shader);
                    }
                };

                // Submit the command
                m_command_queue.SubmitCommand(cmd);
            }
        }

    private:
        Coordinator& m_coordinator;
        CommandQueue& m_command_queue;
    };
}

#endif // SPARK_RENDER_SYSTEM_HPP