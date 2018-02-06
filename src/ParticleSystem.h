#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "GameTime.h"
#include "Model.h"
#include "Shader.h"
#include "Transform.h"
#include "AssortedUtils.h" // for random function

struct Particle 
{
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 force;

	float mass;
};

struct CollisionPlane
{
	glm::vec3 offset;
	glm::vec3 normal;
};

struct CollisionSphere
{
	glm::vec3 centre;
	float radius;
};

class ParticleSystem
{
public:
	ParticleSystem(
		glm::vec3 spawnPosition, 
		glm::vec3 spawnVariance, 
		int numParticles, 
		glm::vec3 initialForce, 
		float initialMass = 1
	);
	~ParticleSystem();

	int m_numParticles;
	Clock* m_pClock;

	glm::vec3 gravity;
	float dragCoefficient;

	void Tick();
	void Draw(Shader* pShader);
	void SetupVAO();


	GLuint VAO, VertexBuffer, PositionsBuffer, ColoursBuffer, VelocitiesBuffer;

	// Data oriented vectors
	vector<glm::vec3> pvec_position;
	vector<glm::vec3> pvec_velocity;
	vector<glm::vec3> pvec_force;
	vector<glm::vec3> pvec_colour;

	vector<float> pvec_mass;
	vector<float> pvec_remainingLife;

	vector<CollisionPlane> planes;
	vector<CollisionSphere> spheres;

	const glm::vec3 waterColour = glm::vec3(0.2f, 0.5f, 0.8f);

};