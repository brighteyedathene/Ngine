#pragma once

#include "Transform.h"

class Camera
{
public:
	Transform transform;
	Camera();
	~Camera();

	glm::mat4 GetViewProjectionMatrix();
};

