#version 450 core

out vec4 FragColor;

in vec4 vertexColor;
in vec2 texCoords;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;
uniform sampler2D texture_diffuse5;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;
uniform sampler2D texture_specular3;
uniform sampler2D texture_specular4;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float blend;

void main()
{
	FragColor = vertexColor * texture(texture1, texCoords);
	//FragColor = vertexColor * mix(texture(texture1, texCoords), texture(texture2, texCoords), blend);
	//FragColor = vec4(0.8f, 0.8f, 0.1f, 1.0f);
}
