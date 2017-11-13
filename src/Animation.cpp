#include "Animation.h"



void Animation::Initialize(int numJoints, int numKeyframes, float duration)
{
	this->duration = duration;

	// set poses array length foreach keyframe so they can be quickly filled (...i think)
	keyframes.resize(numKeyframes);
	for (int i = 0; i < numKeyframes; i++)
		keyframes[i].jointPoses.resize(numJoints);

	// initialize jointsUsed as all false
	jointsUsed.resize(numJoints);
	for (int i = 0; i < numJoints; i++)
		jointsUsed[i] = false;

}