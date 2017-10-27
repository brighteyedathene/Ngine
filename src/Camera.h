#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#include "Transform.h"
#include "Input.h"

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

// Default camera values
const float YAW = 90.0f;
const float PITCH = 0.0f;
const float SPEED = 0.05f;
const float SENSITIVTY = 2.5f;
const float FOV = 45.0f;

class Camera
{
public:
	bool orthographic;
	float fov;

	float nearClipDistance;
	float farClipDistance;

	Transform transform;
	Camera(bool orthographic = false);
	~Camera();

	glm::mat4 GetViewProjectionMatrix();
};

