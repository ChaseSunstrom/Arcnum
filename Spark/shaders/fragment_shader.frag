#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

// Allowed custom includes for different shader files
#include "material.frag"
#include "lighting.frag"

uniform Material material;
uniform vec3 viewPos;

void main() {
    vec4 material_color = material.color;
    FragColor = vec4(material_color.x, material_color.y, material_color.z, material_color.w);
}


