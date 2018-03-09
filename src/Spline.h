#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "Model.h"
#include "Transform.h"

class Spline
{
public:
	Spline(int numberOfSegments);
	~Spline();

	glm::vec3 Sample(float t);

	int segments;
	std::vector<glm::vec3> controlPoints;


	Model* model;

	int drawSamples = 100;
	void Draw(Shader* pShader);

	void Smoothen();
};

