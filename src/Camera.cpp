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
glm::mat4 Camera::GetProjectionViewMatrix()
{
	glm::mat4 view = glm::lookAt(transform.position, transform.position + transform.Forward(), WORLD_UP);
	glm::mat4 projection;

	// TODO get the viewport size in a better way
	GLint vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	float aspect = (float)vp[2] / vp[3];

	//std::cout << "0:" << vp[0] << "  1: "<< vp[1] << "  2: " << vp[2] << "  3:" << vp[3] << std::endl;
	//char c;
	//std::cin >> c;

	if (orthographic)
	{
		//projection = glm::ortho((float)vp[0], (float)vp[2], (float)vp[1], (float)vp[3], -1.0f, 1.0f);
		projection = glm::ortho(-1.0f, 2.0f, -1.0f, 2.0f, -10.0f, 10.0f);
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