#ifndef SPARK_RENDER_STATE_HPP
#define SPARK_RENDER_STATE_HPP

#include "spark_pch.hpp"
#include "spark_shader.hpp"
#include "spark_mesh.hpp"

namespace spark
{
    // Represents a unique render state for batching and state sorting
    struct RenderState
    {
        IShaderProgram* shader_program = nullptr;
        IMesh* mesh = nullptr;
        i32 draw_mode = 0;
        bool depth_test = true;
        bool blending = false;
        bool wireframe = false;

        bool operator==(const RenderState& other) const
        {
            return shader_program == other.shader_program &&
                   mesh == other.mesh &&
                   draw_mode == other.draw_mode &&
                   depth_test == other.depth_test &&
                   blending == other.blending &&
                   wireframe == other.wireframe;
        }
    };
}

// Custom hash function for RenderState
namespace std
{
    template<>
    struct hash<spark::RenderState>
    {
        size_t operator()(const spark::RenderState& state) const
        {
            size_t h1 = std::hash<void*>{}(static_cast<void*>(state.shader_program));
            size_t h2 = std::hash<void*>{}(static_cast<void*>(state.mesh));
            size_t h3 = std::hash<int32_t>{}(state.draw_mode);
            size_t h4 = std::hash<bool>{}(state.depth_test);
            size_t h5 = std::hash<bool>{}(state.blending);
            size_t h6 = std::hash<bool>{}(state.wireframe);
            
            // Combine hashes using FNV-1a inspired approach
            return ((((h1 ^ (h2 << 1)) ^ (h3 << 2)) ^ (h4 << 3)) ^ (h5 << 4)) ^ (h6 << 5);
        }
    };
}

#endif // SPARK_RENDER_STATE_HPP