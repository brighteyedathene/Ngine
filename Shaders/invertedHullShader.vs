#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 5) in vec3 aColour;


uniform mat4 projectionview;
uniform mat4 model;

uniform float thicknessMin;
uniform float thicknessMax;

out vec3 Normal;
out vec3 FragPos;

void main()
{

    Normal = mat3(transpose(inverse(model))) * -aNormal;

	float thickness = thicknessMin + (thicknessMax-thicknessMin )* aColour.r;
	vec3 extendedPos = aPos;
	//extendedPos = extendedPos + Normal * thickness;
	extendedPos = extendedPos * (1+thickness);

    FragPos = vec3(model * vec4(extendedPos, 1.0f));



    gl_Position = projectionview * model * vec4(extendedPos, 1.0);

} 