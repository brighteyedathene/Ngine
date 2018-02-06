#version 450

layout (location = 0) in vec3 aVertexPos;
layout (location = 1) in vec3 aParticlePos;
layout (location = 2) in vec3 aParticleColour;
layout (location = 3) in vec3 aParticleVelocity;


out vec3 particleColour;
out vec2 texCoords;

uniform mat4 projectionview;
uniform vec3 cameraRight;
uniform vec3 cameraUp;

void main()
{
	//TexCoords = aTexCoord;

	vec3 finalVertexPosition = aParticlePos 
								+ cameraRight * aVertexPos.x
								+ cameraUp * aVertexPos.y;

	gl_Position =  projectionview * vec4(finalVertexPosition, 1.0);

	texCoords = aVertexPos.xy + vec2(0.5, 0.5);
	particleColour = aParticleColour;
}