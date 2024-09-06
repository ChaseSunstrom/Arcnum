#version 330 core
in vec2 v_texcoords;
out vec4 frag_color;
uniform sampler2D u_screen_texture;

void main()
{
    frag_color = texture(u_screen_texture, v_texcoords);
}