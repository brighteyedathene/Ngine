#include "AssortedUtils.h"


glm::mat4 GetGlmMat4FromAssimp(aiMatrix4x4 from)
{
	glm::mat4 to;
	to[0][0] = from.a1; to[1][0] = from.a2;
	to[2][0] = from.a3; to[3][0] = from.a4;
	to[0][1] = from.b1; to[1][1] = from.b2;
	to[2][1] = from.b3; to[3][1] = from.b4;
	to[0][2] = from.c1; to[1][2] = from.c2;
	to[2][2] = from.c3; to[3][2] = from.c4;
	to[0][3] = from.d1; to[1][3] = from.d2;
	to[2][3] = from.d3; to[3][3] = from.d4;
	return to;
}

void print_matrix(std::string name, glm::mat4 mat)
{
	float dArray[16] = { 0.0 };
	const float *pSource = (const float*)glm::value_ptr(mat);
	for (int i = 0; i < 16; ++i)
		dArray[i] = pSource[i];

	std::cout << name << ":" << std::endl;
	std::cout << std::fixed;
	std::cout << std::setprecision(2);
	for (int i = 0; i < 4; i++)
	{
		std::cout << "[ ";
		for (int j = 0; j < 4; j++)
		{
			if (pSource[i + j * 4] >= 0)
				std::cout << " ";
			std::cout << pSource[i + j * 4] << " ";
		}
		std::cout << "]" << std::endl;
	}
}

void GetJointTransformFromMat4(glm::mat4 m, glm::vec3 pos, glm::quat rot, glm::vec3 sca)
{
	pos.x = m[0][3]; 
	pos.y = m[1][3]; 
	pos.z = m[2][3];
	m[0][3] = 0; 
	m[1][3] = 0;
	m[2][3] = 0;

	glm::vec3 xscalev = glm::vec3(m[0]);
	glm::vec3 yscalev = glm::vec3(m[1]);
	glm::vec3 zscalev = glm::vec3(m[2]);
	sca.x = glm::length(xscalev);
	sca.y = glm::length(yscalev);
	sca.z = glm::length(zscalev);
	m[0][0] /= sca.x;  m[1][0] /= sca.y;  m[2][0] /= sca.z;
	m[0][1] /= sca.x;  m[1][1] /= sca.y;  m[2][1] /= sca.z;
	m[0][2] /= sca.x;  m[1][2] /= sca.y;  m[2][2] /= sca.z;

	// this isn't finished 
	// i just gave up because i figured out a better way that 
	// doesn't involve writign hard functions
	// m shuold be a simple rotation matrix at this point
	// which can be turned into a quaternjion
	// i'm just not interested in trusting this function
	// https://math.stackexchange.com/questions/237369/given-this-transformation-matrix-how-do-i-decompose-it-into-translation-rotati
}