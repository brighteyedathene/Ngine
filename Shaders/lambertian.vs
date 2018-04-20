#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 projectionview;
uniform mat4 model;

out vec3 Normal;
out vec3 FragPos;


void main()
{
    gl_Position = projectionview * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0f));
    Normal = mat3(transpose(inverse(model))) * aNormal;
} 