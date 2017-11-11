#include "CameraController.h"



CameraController::CameraController(Camera* p_camera, Input* p_input, bool constrainPitch)
{
	this->p_camera = p_camera;
	this->p_input = p_input;
	this->constrainPitch = constrainPitch;
	this->mouseSensitivity = DEFAULT_MOUSE_SENSITIVITY;
}


CameraController::~CameraController()
{
}

void CameraController::Tick()
{
	// movement
	if (p_input->GetButton("Forward"))
		p_camera->transform.position += p_camera->transform.Forward() * SPEED;
	if (p_input->GetButton("Backward"))
		p_camera->transform.position -= p_camera->transform.Forward() * SPEED;
	if (p_input->GetButton("Left"))
		p_camera->transform.position -= p_camera->transform.Right() * SPEED;
	if (p_input->GetButton("Right"))
		p_camera->transform.position += p_camera->transform.Right() * SPEED;
	if (p_input->GetButton("Up"))
		p_camera->transform.position += p_camera->transform.Up() * SPEED;
	if (p_input->GetButton("Down"))
		p_camera->transform.position -= p_camera->transform.Up() * SPEED;

	// rotation
	float xoffset = (float)p_input->mouse_xrel * mouseSensitivity;
	float yoffset = (float)p_input->mouse_yrel * mouseSensitivity;

	p_camera->transform.rotation.y += xoffset;
	p_camera->transform.rotation.x -= yoffset;

	if (constrainPitch)
	{
		if (p_camera->transform.rotation.x > 89.0f)
			p_camera->transform.rotation.x = 89.0f;
		if (p_camera->transform.rotation.x < -89.0f)
			p_camera->transform.rotation.x = -89.0f;
	}


	// Zoom
	if (p_input->mouse_wheel_y != 0)
	{
		if (p_camera->orthographic)
		{
			p_camera->orthoScale = std::fmax(1.0f, p_camera->orthoScale + p_input->mouse_wheel_y * SCALESTEP);
		}
		else
		{
			p_camera->fov = std::fmax(1.0f, p_camera->fov - p_input->mouse_wheel_y * ZOOMSTEP);
			std::cout << "fov" << p_camera->fov << std::endl;
		}
	}
}

