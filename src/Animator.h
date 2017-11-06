#pragma once

#include <map>
#include <vector>
#include <string>


#include "AnimatedModel.h"
#include "GameTime.h"


class Animator
{
public:
	AnimatedModel* m_pModel;
	map<std::string, int> m_animationMap;

	vector<glm::mat4> m_currentMatrices;

	Animator(AnimatedModel* pModel);
	~Animator();

	void Tick(float deltaTime);

private:
	float m_currentTime;

	void GetNextPose();
};
