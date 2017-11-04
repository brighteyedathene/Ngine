#pragma once

#include <map>
#include <vector>
#include <string>


#include "AnimatedModel.h"
#include "GameTime.h"

struct AnimationSample
{
	float t;
	vector<JointPose> jointPoses;
};

struct AnimationClip
{
	float duration;
	vector<AnimationSample> samples;
};


class Animator
{
public:

	AnimatedModel model;
	map<std::string, int> animationMap;
	vector<AnimationClip> animations;

	Animator();
	~Animator();
};
