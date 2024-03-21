#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in mat4 aInstanceMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 model = aInstanceMatrix; 
    mat4 modelView = view * model;
    FragPos = vec3(modelView * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(modelView))) * aNormal;
    TexCoords = aTexCoords;
    
    gl_Position = projection * modelView * vec4(aPos, 1.0);
}
