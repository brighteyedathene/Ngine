#version 450 core

out vec4 FragColor;

in vec3 Colour;

void main()
{
	FragColor = vec4(1, 1- Colour.r, 1 -Colour.r, 1);
}
