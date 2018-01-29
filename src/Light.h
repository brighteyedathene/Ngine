#pragma once


#include "Transform.h"

struct Light
{
	Transform transform;
	glm::vec3 colour;
};