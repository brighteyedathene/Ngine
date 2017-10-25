#pragma once

#include "Camera.h"
#include "Input.h"


// Defaults
const float DEFAULT_MOUSE_SENSITIVITY = 0.5f;

class CameraController
{
public:
	float mouseSensitivity;
	bool constrainPitch;

	CameraController(Camera* p_camera, Input* p_input, bool constrainPitch = true);
	~CameraController();

	void Tick();

private:
	Camera* p_camera;
	Input* p_input;



};

