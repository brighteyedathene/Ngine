#pragma once


#include "Transform.h"

struct Light
{
	Transform transform;
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};