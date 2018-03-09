#pragma once


#include <glm/glm.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp>

#include <vector>

#include "transform.h"
#include "Model.h"


class IKJointConstraint
{
public:
	float up    = 20.0f;
	float down  = 20.0f;
	float right = 20.0f;
	float left  = 20.0f;
};

class IKJoint
{
public:
	Transform transform;
	float length = 5;
	float degreesOfFreedom = 30.0f;

	IKJointConstraint dof;
};



class IKFrame
{
public:
	IKFrame();
	~IKFrame();

	Transform transform;

	Model* jointModel;

	glm::vec3 goal;
	bool goalReachable;
	float threshold = 0.01f;
	int maxIterations = 10;

	int numJoints;
	std::vector<IKJoint> joints;
	std::vector<glm::vec3> points;

	void UpdateIK();
	void Draw(Shader* pShader);

private:
	void DoFABRIK();
	void StraightenOut();

	void ReachBackwards();
	void ReachForwards();
	void ReachForwardsAndConstrain();

	glm::vec3 Constrain(glm::vec3 desiredDirection, glm::vec3 oldDirection, IKJointConstraint dof);

	// Match joint positions to current points
	void UpdateJoints();

	// Update joints but apply constraints
	void UpdateJointsWithConstraints();
};

