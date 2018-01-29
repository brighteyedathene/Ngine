#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 mvp;
uniform vec3 ucolour;

out vec3 colour;

void main()
{
    gl_Position = mvp * vec4(aPos, 1.0);
    colour = ucolour;
} 