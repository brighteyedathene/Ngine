#pragma once

#include <map>
#include <vector>
#include <string>


#include "AnimatedModel.h"
#include "GameTime.h"

const int MAX_JOINTS = 70;

class Animator
{
public:
	AnimatedModel* m_pModel;
	map<std::string, int> m_animationMap;

	glm::mat4* m_currentMatrices;
	int m_jointCount;

	Animator(AnimatedModel* pModel);
	~Animator();
	void Tick(float deltaTime);

private:
	float m_currentTime;
	glm::mat4* m_modelSpaceTransforms;

	void GetNextPose();
};
