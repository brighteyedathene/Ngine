#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <sstream>

// World-space right vector
const glm::vec3 WORLD_RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);
// World-space up vector
const glm::vec3 WORLD_UP = glm::vec3(0.0f, 1.0f, 0.0f);
// World-space forward vector
const glm::vec3 WORLD_FORWARD = glm::vec3(0.0f, 0.0f, 1.0f);

class Transform
{
public:

	glm::vec3 position;
	glm::vec3 rotation; // TODO change to quat
	glm::vec3 scale;
	glm::mat4 GetMatrix();

	glm::vec3 Forward();
	glm::vec3 Right();
	glm::vec3 Up();

	Transform();
	~Transform();

	std::string ToString();

	inline glm::vec3* GetPosition() { return &position; }
	inline glm::vec3* GetRotation() { return &rotation; }
	inline glm::vec3* GetScale() { return &scale; }

	inline void SetPosition(glm::vec3 value) { this->position = value; }
	inline void SetRotation(glm::vec3 value) { this->rotation = value; }
	inline void SetScale(glm::vec3 value) { this->scale = value; }


};

#endif