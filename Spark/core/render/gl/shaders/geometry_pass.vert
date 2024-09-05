#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_normal;
layout (location = 2) in vec2 a_texcoords;

out vec3 v_frag_pos;
out vec3 v_normal;
out vec2 v_texcoords;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    v_frag_pos = vec3(u_model * vec4(a_pos, 1.0));
    v_normal = mat3(transpose(inverse(u_model))) * a_normal;
    v_texcoords = a_texcoords;
    
    gl_Position = u_projection * u_view * vec4(v_frag_pos, 1.0);
}