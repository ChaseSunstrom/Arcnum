#version 330 core
out vec4 frag_color;

in vec2 v_texcoords;

uniform sampler2D u_screen_texture;

void main()
{
    vec3 color = texture(u_screen_texture, v_texcoords).rgb;
    
    // Simple grayscale effect
    float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
    frag_color = vec4(vec3(average), 1.0);
}