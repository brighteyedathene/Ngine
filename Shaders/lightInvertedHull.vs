#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

layout (location = 5) in vec3 aColour;


struct Light {
    vec3 position;
    vec3 colour;
};
uniform Light light; 

uniform vec3 viewPos;

uniform mat4 projectionview;
uniform mat4 model;

uniform float thicknessMin;
uniform float thicknessMax;

out vec3 Normal;
out vec3 FragPos;


out float lambert;

void main()
{

    Normal = mat3(transpose(inverse(model))) * aNormal;
    //Normal = mat3(model) * aNormal;

    vec3 Pos = vec3(model * vec4(aPos, 1.0f));

	vec3 lightDir = light.position - Pos;
	vec3 viewDir = Pos - viewPos;
	float distance = length(viewDir);

	float lambertian = max(dot(Normal, lightDir), 0.0);

	float thickness = thicknessMin + (thicknessMax - thicknessMin) * (1 - lambertian);

	Pos = Pos + Normal * thickness * sqrt(distance) * 2;
	//extendedPos = extendedPos * (1+thickness);

    FragPos = Pos;

    lambert = lambertian;

    gl_Position = projectionview * vec4(Pos, 1.0);

} 