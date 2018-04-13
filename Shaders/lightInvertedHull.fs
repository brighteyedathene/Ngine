#version 450

out vec4 FragColor;


uniform vec3 lineColour;


void main()
{
    FragColor = vec4(lineColour, 1);
}