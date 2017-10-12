#version 450 core

out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoords;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float blend;

void main()
{
	//FragColor = vertexColor * texture(texture1, texCoords);
	FragColor = vertexColor * mix(texture(texture1, texCoords), texture(texture2, texCoords), blend);
}
