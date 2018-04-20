#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

layout (location = 5) in vec3 aColour;
 
uniform mat4 projectionview;
uniform mat4 model;

uniform vec3 viewPos;

out vec3 Normal;
out vec3 FragPos;

out float viewDotNormal;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0f));
    Normal = normalize(mat3(transpose(inverse(model))) * aNormal);

    vec3 viewDirection = FragPos - viewPos;
    viewDotNormal = dot(-viewDirection, Normal);
    viewDotNormal /= length(viewDirection);

    gl_Position = projectionview * vec4(FragPos, 1.0);
} 