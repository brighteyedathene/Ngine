#pragma once

#include <map>
#include <vector>
#include <string>


#include "AnimatedModel.h"
#include "GameTime.h"

struct AnimationSample
{
	float t;

};

struct AnimationClip
{
	float duration;

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
