#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

struct Light {
    vec3 position;
    //vec3 colour;
};
uniform Light light; 

uniform vec3 viewPos;

uniform mat4 projectionview;
uniform mat4 model;

uniform float thicknessMin;
uniform float thicknessMax;

out vec3 Normal;
out vec3 FragPos;


void main()
{

	// Apply model matrix transformation to position and normal
    Normal = mat3(transpose(inverse(model))) * aNormal;
    vec3 Pos = vec3(model * vec4(aPos, 1.0f));

    // Find lambertian value
	vec3 lightDir = light.position - Pos;
	float lambertian = max(dot(Normal, lightDir), 0.0);

	// Calculate thickness based on lambertian value
	float thickness = thicknessMin + (thicknessMax - thicknessMin) * (1 - lambertian);

	// Calculate view direction and distance
	vec3 viewDir = Pos - viewPos;
	float distance = length(viewDir);
	viewDir = normalize(viewDir);

	// Calculate vector by which to extrude the vertex
	vec3 extrudeVector = Normal * thickness;
	vec3 orthogonalProjection = extrudeVector - dot(extrudeVector, viewDir) * viewDir;

	// Apply the extrusion
	Pos = Pos + orthogonalProjection * sqrt(distance) * 2;

    FragPos = Pos;
    gl_Position = projectionview * vec4(Pos, 1.0);

} 