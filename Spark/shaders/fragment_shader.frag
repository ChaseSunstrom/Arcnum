#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

#include "material.frag"

uniform Material material;

void main() {
    vec4 material_color = material.color;
    FragColor = vec4(material_color.x, material_color.y, material_color.z, material_color.w);
}


