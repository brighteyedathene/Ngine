#include "ParticleSystem.h"



ParticleSystem::ParticleSystem(
	glm::vec3 spawnPosition, 
	glm::vec3 spawnVariance, 
	int numParticles, 
	glm::vec3 initialForce, 
	float initialMass)
{
	m_numParticles = numParticles;

	// resize particle vectors
	pvec_position.resize(m_numParticles);
	pvec_velocity.resize(m_numParticles);
	pvec_force.resize(m_numParticles);
	pvec_colour.resize(m_numParticles);
	
	pvec_mass.resize(m_numParticles);
	pvec_remainingLife.resize(m_numParticles);

	// initialise particle vectors
	for (int i = 0; i < numParticles; i++)
	{
		pvec_position[i] = glm::vec3(
			spawnPosition.x + Randf() * spawnVariance.x,
			spawnPosition.y + Randf() * spawnVariance.y,
			spawnPosition.z + Randf() * spawnVariance.z
		);
		pvec_velocity[i] = glm::vec3(0.0);
		pvec_force[i] = initialForce;
		pvec_mass[i] = initialMass + Randf();

		pvec_colour[i] = glm::vec3(0.2, 0.4, 1.0);
		pvec_remainingLife[i] = 5.0f;

	}


	// Initialize forces
	gravity = glm::vec3(0.0f, -9.8f, 0.0f);
	dragCoefficient = 0.4;


	CollisionPlane plane;
	plane.offset = glm::vec3(0.0, -100.0, 0.0);
	plane.normal = glm::vec3(0.5, 1.0, 0.0);
	//planes.push_back(plane);

	CollisionPlane plane2;
	plane2.offset = glm::vec3(0.0, -300.0, 0.0);
	plane2.normal = glm::vec3(-0.2, 1.0, 0.0);
	planes.push_back(plane2);

	CollisionSphere sphere;
	sphere.centre = glm::vec3(5.0, 5.0, 5.0);
	sphere.radius = 10.0f;
	//spheres.push_back(sphere);

	SetupVAO();
}


ParticleSystem::~ParticleSystem()
{
}



void ParticleSystem::Tick()
{
	float deltaTime = (float)m_pClock->deltaTime;

	for (int i = 0; i < m_numParticles; i++)
	{
		// Add gravity
		pvec_force[i] += gravity * pvec_mass[i];
	
		// Add drag
		pvec_force[i] -= dragCoefficient * pvec_velocity[i];

		// Apply forces
		glm::vec3 newVelocity = pvec_velocity[i] + (pvec_force[i] / pvec_mass[i]) * deltaTime;
		glm::vec3 newPosition = pvec_position[i] + pvec_velocity[i] * deltaTime;

		// Check collision
		// planes
		for (int j = 0; j < planes.size(); j++)
		{
			float intersectingDot = glm::dot((newPosition - planes[j].offset), planes[j].normal);
			float penetratingDot = glm::dot(planes[j].normal, newVelocity);
			if (intersectingDot < 0.01 && penetratingDot < 0.01)
			{
				newVelocity = newVelocity - 2 * penetratingDot * planes[j].normal;
				pvec_colour[i] = glm::vec3(1.0);
			}
		}
		// spheres
		for (int j = 0; j < spheres.size(); j++)
		{
			glm::vec3 difference = newPosition - spheres[j].centre;
			float distance = glm::length(difference);
			if (distance < spheres[j].radius)
			{

				glm::vec3 collisionNormal = glm::normalize(difference);
				//float penetratingDot = glm::dot(collisionNormal, newVelocity);
				//newVelocity = newVelocity - 2 * penetratingDot * collisionNormal * 0.8f;
				newVelocity = glm::reflect(newVelocity, collisionNormal);
				difference = spheres[j].centre - pvec_position[i] + newVelocity * deltaTime;
				if (glm::length(difference) < spheres[j].radius)
				{
					newVelocity = glm::normalize(-difference) * spheres[j].radius;
				}

				pvec_colour[i] = glm::vec3(1.0);
			}

		}

		pvec_velocity[i] = newVelocity;
		pvec_position[i] += newVelocity * deltaTime;

		// Zero the force
		pvec_force[i] = glm::vec3(0.0);

		// extras
		pvec_remainingLife[i] -= deltaTime;

		// fade back to blue
		pvec_colour[i].r = max(waterColour.r, pvec_colour[i].r - deltaTime * 0.08f);
		pvec_colour[i].g = max(waterColour.g, pvec_colour[i].g - deltaTime * 0.05f);
		pvec_colour[i].b = max(waterColour.b, pvec_colour[i].b - deltaTime * 0.03f);
	}
}


void ParticleSystem::Draw(Shader* pShader)
{
	// Draw the Particles

	glm::mat4 model_matrix = glm::mat4(1.0f);

	
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, PositionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numParticles * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_numParticles * sizeof(GLfloat) * 3, &pvec_position[0]);
	
	glBindBuffer(GL_ARRAY_BUFFER, ColoursBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numParticles * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_numParticles * sizeof(GLfloat) * 3, &pvec_colour[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VelocitiesBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numParticles * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); 
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_numParticles * sizeof(GLfloat) * 3, &pvec_velocity[0]);


	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glVertexAttribPointer(
		0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
		3,                  // size
		GL_FLOAT,           // type
		GL_FALSE,           // normalized?
		0,                  // stride
		(void*)0            // array buffer offset
	);

	// 2nd attribute buffer : positions of particles' centers
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, PositionsBuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size : x + y + z
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 3rd attribute buffer : particles' colors
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, ColoursBuffer);
	glVertexAttribPointer(
		2,                                // attribute. No particular reason for 2, but must match the layout in the shader.
		3,                                // size : r + g + b
		GL_FLOAT,							// type
		GL_FALSE,                          // normalized? 
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	// 2nd attribute buffer : velocities of particles
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, VelocitiesBuffer);
	glVertexAttribPointer(
		3,
		3,
		GL_FLOAT,
		GL_FALSE,
		0,
		(void*)0
	);


	glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
	glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
	glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1
	glVertexAttribDivisor(3, 1); // velocity

	glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, m_numParticles);


	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(3);

}




void ParticleSystem::SetupVAO()
{
	float quad[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	// VBO containing the positions of the particles
	glGenBuffers(1, &PositionsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, PositionsBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numParticles * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	// VBO containing the colors of the particles
	glGenBuffers(1, &ColoursBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ColoursBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numParticles * 3 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

	// VBO containing the velocities of the particles
	glGenBuffers(1, &VelocitiesBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VelocitiesBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_numParticles * 3 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);

}