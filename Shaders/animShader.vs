#version 450

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 aJointIDs;
layout (location = 4) in vec4 aWeights;

//out vec2 TexCoord0; // put this in later
out vec3 Normal;
out vec3 FragPos;

smooth out float DEBUG_ZERO_WEIGHT;

const int MAX_JOINTS = 50;

uniform mat4 projectionview;
uniform mat4 model;
uniform mat4 joints[MAX_JOINTS];

void main()
{
	mat4 jointTransform = joints[aJointIDs[0]] * aWeights[0];
	jointTransform += joints[aJointIDs[1]] * aWeights[1];
	jointTransform += joints[aJointIDs[2]] * aWeights[2];
	jointTransform += joints[aJointIDs[3]] * aWeights[3];

	vec4 position = jointTransform * vec4(aPos, 1.0);
	gl_Position = projectionview * model * position;
	//gl_Position = projectionview * model * vec4(aPos, 1.0);

	//TexCoord0 = aTexCoord;
	vec4 norm = jointTransform * vec4(aNormal, 0.0);
	
	//Normal = (model * norm).xyz;
	//FragPos = (model * position).xyz;

	Normal = mat3(transpose(inverse(model))) * aNormal;
	FragPos = vec3(model * vec4(aPos, 1.0f));

	DEBUG_ZERO_WEIGHT = 0;
	for (int i = 0; i < 4; i++)
	{
		if (aWeights[i] == 0)
		{
			DEBUG_ZERO_WEIGHT = 1;
		}
	}

}
