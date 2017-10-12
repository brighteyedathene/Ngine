#include "Transform.h"


glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);

Transform::Transform()
{
	position = glm::vec3(0.0f);
	scale = glm::vec3(1.0f);
	rotation = glm::vec3(0.0f);
}


Transform::~Transform()
{
}

glm::mat4 Transform::GetMatrix()
{
	glm::mat4 mat = glm::mat4(1.0f);
	mat = glm::scale(mat, scale);

	mat = glm::rotate(mat, rotation.x, right);
	mat = glm::rotate(mat, rotation.y, up);
	mat = glm::rotate(mat, rotation.z, forward);

	mat = glm::translate(mat, position);
	return mat;
}