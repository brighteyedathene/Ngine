#pragma once

#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>



	// copy from Row-major to Column major matrix
	// i.e. from aiMatrix4x4 to glm::mat4
	template <typename RM, typename CM>
	void AssToGlmMat4(const RM& from, CM& to)
	{
		to[0][0] = from.a1; to[1][0] = from.a2;
		to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2;
		to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2;
		to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2;
		to[2][3] = from.d3; to[3][3] = from.d4;
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
