#pragma once

#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/matrix_decompose.hpp>

#include <assimp/scene.h>


#ifdef WIN32
#define SNPRINTF _snprintf_s
#else
#define SNPRINTF snprintf
#endif

glm::mat4 GetGlmMat4FromAssimp(aiMatrix4x4 from);

void print_matrix(std::string name, glm::mat4 mat);


inline glm::quat EulerToQuat(glm::vec3 euler)
{
	glm::quat q;

	float cosYaw = cos(glm::radians(euler.y));
	float cosPitch = cos(glm::radians(euler.x));
	float cosRoll = cos(glm::radians(euler.z));
	float sinYaw = sin(glm::radians(euler.y));
	float sinPitch = sin(glm::radians(euler.x));
	float sinRoll = sin(glm::radians(euler.z));

	q.w = sqrt(1.0 + cosYaw * cosPitch + cosYaw * cosRoll - sinYaw * sinPitch * sinRoll + cosPitch * cosRoll) / 2;
	q.x = (cosPitch * sinRoll + cosYaw * sinRoll + sinYaw * sinPitch * cosRoll) / (4.0 * q.w);
	q.y = (sinYaw * cosPitch + sinYaw * cosRoll + cosYaw * sinPitch * sinRoll) / (4.0 * q.w);
	q.z = (-sinYaw * sinRoll + cosYaw * sinPitch * cosRoll + sinPitch) / (4.0 * q.w);

	glm::normalize(q);

	return q;
}