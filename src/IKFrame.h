#pragma once


#include <glm/glm.hpp>
#include <vector>

#include "transform.h"

class IKJoint
{
public:
	Transform transform;
	float length = 1;
};


class IKFrame
{
public:
	IKFrame();
	~IKFrame();

	glm::vec3 goal;
	glm::vec3 root;
	bool goalReachable;

	int numJoints;
	std::vector<IKJoint> joints;
	std::vector<glm::vec3> points;

	void UpdateIK();
};

