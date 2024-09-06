#version 330 core
layout (location = 0) out vec3 g_position;
layout (location = 1) out vec3 g_normal;
layout (location = 2) out vec4 g_albedo;

in vec3 v_frag_pos;
in vec3 v_normal;
in vec2 v_texcoords;

uniform sampler2D u_texture_diffuse1;
uniform vec4 u_color; // Fallback color

void main()
{
    g_position = v_frag_pos;
    g_normal = normalize(v_normal);
    
    // Use texture if available, otherwise use fallback color
    if (textureSize(u_texture_diffuse1, 0).x > 1)
        g_albedo = texture(u_texture_diffuse1, v_texcoords).rgba;
    else
        g_albedo = u_color;
}