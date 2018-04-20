#version 450 core
layout (location = 0) in vec3 aPos;

layout (location = 5) in vec3 aColour;

uniform mat4 projectionview;
uniform mat4 model;

out vec3 Colour;

void main()
{
    gl_Position = projectionview * model * vec4(aPos, 1.0f);
    Colour = aColour;

}
