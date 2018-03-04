#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec3 aColour;

uniform mat4 projectionview;
uniform mat4 model;

out vec3 Normal;
out vec3 FragPos;

out vec3 vColour;

void main()
{
    gl_Position = projectionview * model * vec4(aPos, 1.0);
    FragPos = vec3(model * vec4(aPos, 1.0f));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    vColour = aColour;
} 