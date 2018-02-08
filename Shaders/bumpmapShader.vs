#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

out mat3 TBN;

uniform mat4 projectionview;
uniform mat4 model;

void main()
{
	//TBN = mat3(aTangent, aBitangent, aNormal);
	vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
   	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
   	vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
   	TBN = mat3(T, B, N);

	TexCoords = aTexCoord;
	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	gl_Position =  projectionview * model * vec4(aPos, 1.0);
}