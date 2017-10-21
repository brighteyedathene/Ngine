#include "Transform.h"


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
	mat = glm::rotate(mat, glm::radians(rotation.y), WORLD_UP);
	mat = glm::rotate(mat, glm::radians(rotation.x), WORLD_RIGHT);
	mat = glm::rotate(mat, glm::radians(rotation.z), WORLD_FORWARD);

	glm::mat4 mat_scale = glm::scale(mat, scale);
	
	//mat = mat_translation * mat_rotation * mat_scale;

	return mat;
}

glm::vec3 Transform::Forward()
{
	glm::vec3 forward;
	forward.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	forward.y = sin(glm::radians(rotation.x));
	forward.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	forward = glm::normalize(forward);
	return forward;
}

glm::vec3 Transform::Right()
{
	glm::vec3 forward;
	forward.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	forward.y = sin(glm::radians(rotation.x));
	forward.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	forward = glm::normalize(forward);
	
	return glm::normalize(glm::cross(forward, WORLD_UP));
}

glm::vec3 Transform::Up()
{
	glm::vec3 forward;
	forward.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	forward.y = sin(glm::radians(rotation.x));
	forward.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
	forward = glm::normalize(forward);

	glm::vec3 right = glm::normalize(glm::cross(forward, WORLD_UP));
	return glm::normalize(glm::cross(right, forward));
}

std::string Transform::ToString()
{
	std::ostringstream out;
	out << "pos: " << position.x << ", " << position.y << ", " << position.z << "\n"
		<< "rot: " << glm::degrees(rotation.x) << ", " << glm::degrees(rotation.y) << ", " << glm::degrees(rotation.z) << "\n"
		<< "sca: " << scale.x    << ", " << scale.y    << ", " << scale.z    << "\n";
	
	return out.str();
}