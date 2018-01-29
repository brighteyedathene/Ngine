#pragma once

#include <glm/glm.hpp>

class Material
{
public:
	Material();
	~Material();

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
};

