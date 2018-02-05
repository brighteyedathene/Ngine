#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 FragPos;
out vec3 Normal;
//out vec3 TexCoords;

uniform mat4 projectionview;
uniform mat4 model;

void main()
{
	//TexCoords = aTexCoord;
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	gl_Position =  projectionview * model * vec4(aPos, 1.0);
}