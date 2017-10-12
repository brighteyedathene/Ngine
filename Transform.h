#pragma once
#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class Transform
{
public:

	glm::vec3 position;
	glm::vec3 rotation; // TODO change to quat
	glm::vec3 scale;


	Transform();
	~Transform();

	glm::mat4 GetMatrix();

	inline glm::vec3* GetPosition() { return &position; }
	inline glm::vec3* GetRotation() { return &rotation; }
	inline glm::vec3* GetScale() { return &scale; }

	inline void SetPosition(glm::vec3 value) { this->position = value; }
	inline void SetRotation(glm::vec3 value) { this->rotation = value; }
	inline void SetScale(glm::vec3 value) { this->scale = value; }

};

#endif