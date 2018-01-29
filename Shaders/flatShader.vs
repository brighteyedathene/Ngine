#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 projectionview;
uniform mat4 model;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

void main()
{
	vec4 vertexPosition = vec4(aPos, 1.0);
	gl_Position = projectionview * model * vertexPosition;

	TexCoords = aTexCoord;
	vec4 norm = vec4(aNormal, 0.0);
	
	Normal = (model * norm).xyz;
	FragPos = (model * vertexPosition).xyz;

	//Normal = mat3(transpose(inverse(model))) * aNormal;
	//FragPos = vec3(model * vec4(aPos, 1.0f));
}
