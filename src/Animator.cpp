#include "Animator.h"



Animator::Animator(AnimatedModel* pModel)
{
	m_pModel = pModel;
	m_currentMatrices.resize(m_pModel->m_Skeleton.m_joints.size());
	m_currentTime = 0;
}


Animator::~Animator()
{
}

//void Animator::PlayAnimation(string name, maybe enum, bool loop, speed?);

void Animator::Tick(float deltaTime)
{
	m_currentTime += deltaTime;
	
	GetNextPose();


}

void Animator::GetNextPose()
{
	// current animation
	Animation anim = m_pModel->m_Skeleton.m_animations[0];

	// Get closest 2 keyframes from animation
	//TODO handle 1 frame and past duration!
	int prev = 0;
	int next = 1;
	while (next < anim.keyframes.size())
	{
		if (anim.keyframes[prev].timestamp < m_currentTime && anim.keyframes[next].timestamp > m_currentTime)
			break;

		prev++;
		next++;
	}

	// Interploate between 2 keyframes across all joint poses
	float difference = anim.keyframes[next].timestamp - anim.keyframes[prev].timestamp;
	float t = m_currentTime - anim.keyframes[prev].timestamp;



	// create array of matrices from joint poses

	// draw something with the matrices
}