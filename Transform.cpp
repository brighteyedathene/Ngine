#include "Transform.h"


const glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);

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

	//glm::mat4 mat_translation = glm::translate(glm::mat4(1.0f), position);
	mat = glm::translate(glm::mat4(1.0f), position);

	//glm::quat rQuat = glm::quat(rotation);
	//glm::mat4 mat_rotation = glm::mat4_cast(rQuat);
	mat = glm::rotate(mat, rotation.y, up);
	mat = glm::rotate(mat, rotation.x, right);
	mat = glm::rotate(mat, rotation.z, forward);

	glm::mat4 mat_scale = glm::scale(mat, scale);
	
	//mat = mat_translation * mat_rotation * mat_scale;

	return mat;
}

std::string Transform::ToString()
{
	std::ostringstream out;
	out << "pos: " << position.x << ", " << position.y << ", " << position.z << "\n"
		<< "rot: " << glm::degrees(rotation.x) << ", " << glm::degrees(rotation.y) << ", " << glm::degrees(rotation.z) << "\n"
		<< "sca: " << scale.x    << ", " << scale.y    << ", " << scale.z    << "\n";
	
	return out.str();
}