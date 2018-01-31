#include "Camera.h"



Camera::Camera(bool orthographic)
{
	this->orthographic = orthographic;
	fov = FOV;
	orthoScale = ORTHOSCALE;
	transform.position = glm::vec3(0.0f, 0.0f, -3.0f);
	transform.rotation = glm::vec3(PITCH, YAW, 0.0f);

	nearClipDistance = 0.01f;
	farClipDistance = 100.0f;
}


Camera::~Camera()
{
}

// premultiply model by this matrix
glm::mat4 Camera::GetProjectionViewMatrix()
{
	glm::mat4 view = glm::lookAt(transform.position, transform.position + transform.Forward(), transform.Up());
	glm::mat4 projection;

	// TODO get the viewport size in a better way
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	float aspect = (float)vp[2] / vp[3];


	if (orthographic)
	{
		float orthoWidth = (float)vp[2] * aspect / orthoScale;
		float orthoHeight = (float)vp[3] * aspect / orthoScale;
		projection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -500.0f, 500.0f);
	}
	else
	{
		projection = glm::perspective(
			glm::radians(fov),
			aspect,
			nearClipDistance,
			farClipDistance
		);
	}
	
	return projection * view;
}

// Basically the same, but uses transform.Up() instead of WORLD_UP
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(transform.position, transform.position + transform.Forward(), transform.Up());
}

glm::mat4 Camera::GetProjectionMatrix()
{
	glm::mat4 projection;

	// TODO get the viewport size in a better way
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	float aspect = (float)vp[2] / vp[3];

	if (orthographic)
	{
		float orthoWidth = (float)vp[2] * aspect / orthoScale;
		float orthoHeight = (float)vp[3] * aspect / orthoScale;
		projection = glm::ortho(-orthoWidth, orthoWidth, -orthoHeight, orthoHeight, -500.0f, 500.0f);
	}
	else
	{
		projection = glm::perspective(
			glm::radians(fov),
			aspect,
			nearClipDistance,
			farClipDistance
		);
	}

	return projection;
}