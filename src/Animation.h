#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

using namespace std;

struct JointPose
{
	glm::quat rotation;
	glm::vec3 position;
	glm::vec3 scale;

	JointPose()
	{
		rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		position = glm::vec3(0.0f);
		scale = glm::vec3(1.0f);
	}
};

struct Keyframe
{
	vector<JointPose> jointPoses;
	float timestamp;
};

struct Animation
{
	vector<Keyframe> keyframes;
	vector<bool> jointsUsed;
	float duration;
	bool loop;

	void Initialize(int numJoints, int numKeyframes, float duration);
};