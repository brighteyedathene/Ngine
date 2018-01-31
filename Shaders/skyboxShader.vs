#version 450

layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projectionview;

void main()
{
	TexCoords = aPos;
	vec4 pos = projectionview * vec4(aPos, 1.0);
	gl_Position = pos.xyww;
}