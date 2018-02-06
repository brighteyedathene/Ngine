#version 450

out vec4 FragColor;

in vec3 particleColour;
in vec3 texCoords;

uniform samplerCube skybox;
uniform vec3 viewPos;



void main()
{
	
	FragColor = vec4(particleColour, 1.0);
	//FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}