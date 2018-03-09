#include "Spline.h"



Spline::Spline(int numberOfSegments)
{
	segments = numberOfSegments;
	controlPoints.resize(1 + 3 * segments);
	for (int i = 0; i < controlPoints.size(); i++)
	{
		controlPoints[i] = glm::vec3(0.0, (float)i, 0.0);
	}

}


Spline::~Spline()
{
}


glm::vec3 Spline::Sample(float t)
{
	t = min(1.0f, max(0.0f, t));

	// Determine which segment to sample, and map t to that segment
	float segSize = (float) 1.0 / segments;
	int segment = (int)(t / segSize);
	segment = min(segment, segments - 1); // sometimes floating points can cause a problem
	t = (t - segment * segSize) / segSize;


	float omt = 1 - t;

	if (segment * 3 + 3 < controlPoints.size())
	{
		return
			omt * omt*omt * controlPoints[segment * 3] +
			3 * omt * omt * t * controlPoints[segment * 3 + 1] +
			3 * omt * t * t * controlPoints[segment * 3 + 2] +
			t * t * t * controlPoints[segment * 3 + 3];
	}
	else
	{
		std::cout << "fuugg, segment was " << segment << " need to get controlpoitns[" << segment * 3 + 3 << " but there's only " << controlPoints.size() << std::endl;
		return glm::vec3(0, 0, 0);
	}
}


void Spline::Draw(Shader* pShader)
{
	// To get transformation matrix easily
	Transform transform;

	//Set up material and shaders
	pShader->Use();

	// Draw guide points
	transform.scale = glm::vec3(0.05);
	for (int i = 0; i < controlPoints.size(); i++)
	{
		transform.position = controlPoints[i];
		pShader->SetMat4("model", transform.GetMatrix());

		if (i % 3)
		{
			pShader->SetVec3("colour", glm::vec3(1.0, 0.0, 0.0));
		}
		else
		{

			pShader->SetVec3("colour", glm::vec3(0.5, 0.0, 0.0));
		}
		model->Draw(pShader);
	}

	// Draw samples
	pShader->SetVec3("colour", glm::vec3(0.0, 0.0, 0.0));
	transform.scale = glm::vec3(0.01); // further decrease the scale
	for (int i = 0; i < drawSamples; i++)
	{

		float t = (float) i / drawSamples;

		transform.position = Sample(t);
		pShader->SetMat4("model", transform.GetMatrix());
		model->Draw(pShader);
	}
}

void Spline::Smoothen()
{
	for (int i = 1; i < segments; i++)
	{
		glm::vec3 inDirection = controlPoints[i * 3] - controlPoints[i * 3 - 1];
		float outDistance = glm::length(controlPoints[i * 3 + 1] - controlPoints[i * 3]);
		//controlPoints[i * 3 + 1] = controlPoints[i * 3] + glm::normalize(inDirection) * outDistance;
		controlPoints[i * 3 + 1] = controlPoints[i * 3] + glm::normalize(inDirection) * glm::length(inDirection);
	}
}