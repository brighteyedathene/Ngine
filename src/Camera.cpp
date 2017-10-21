#include "Camera.h"



Camera::Camera(bool orthographic)
{
	this->orthographic = orthographic;
	fov = FOV;
	transform.position = glm::vec3(0.0f, 0.0f, -3.0f);
	transform.rotation = glm::vec3(PITCH, YAW, 0.0f);

	nearClipDistance = 0.01f;
	farClipDistance = 100.0f;
}


Camera::~Camera()
{
}

// premultiply model by this matrix
glm::mat4 Camera::GetViewProjectionMatrix()
{
	glm::mat4 view = glm::lookAt(transform.position, transform.position + transform.Forward(), WORLD_UP);
	glm::mat4 projection;

	// TODO get the viewport size in a better way
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	float aspect = (float)vp[2] / vp[3];

	if (orthographic)
	{
		projection = glm::ortho(vp[0], vp[2], vp[1], vp[3]);
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