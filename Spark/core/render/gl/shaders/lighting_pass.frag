#version 330 core
out vec4 frag_color;

in vec2 v_texcoords;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;

// TODO: Add light uniforms

void main()
{
    vec3 frag_pos = texture(g_position, v_texcoords).rgb;
    vec3 normal = texture(g_normal, v_texcoords).rgb;
    vec3 albedo = texture(g_albedo, v_texcoords).rgb;
    
    // TODO: Implement lighting calculations
    
    vec3 ambient = 0.1 * albedo;
    
    frag_color = vec4(ambient, 1.0);
}