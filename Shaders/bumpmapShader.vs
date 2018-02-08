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
	// Calculating tangent, bitangent, and assemble TBN matrix
	//vec3 tangent;
	//vec3 cross1 = cross(aNormal, vec3(0.0, 0.0, 1.0));
	//vec3 cross2 = cross(aNormal, vec3(0.0, 1.0, 0.0));
	//if(length(cross1) > length(cross2))
	//{
	//	tangent = cross1;
	//}
	//else
	//{
	//	tangent = cross2;
	//}
	//tangent = normalize(tangent);
	//vec3 bitangent = cross(aTangent, aNormal);


	//TBN = mat3(aTangent, aBitangent, aNormal);
	vec3 T = normalize(vec3(model * vec4(aTangent,   0.0)));
   	vec3 B = normalize(vec3(model * vec4(aBitangent, 0.0)));
   	vec3 N = normalize(vec3(model * vec4(aNormal,    0.0)));
   	TBN = mat3(T, B, N);

	TexCoords = aTexCoord;
	if (length(aTangent) < 0.1){
		TexCoords = vec2(0.0, 0.0);
	}

	FragPos = vec3(model * vec4(aPos, 1.0));
	Normal = normalize(mat3(transpose(inverse(model))) * aNormal);
	gl_Position =  projectionview * model * vec4(aPos, 1.0);
}