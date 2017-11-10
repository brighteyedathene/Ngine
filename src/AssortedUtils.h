#pragma once

#include <iostream>
#include <iomanip>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
//#include <glm/gtx/matrix_decompose.hpp>

#include <assimp/scene.h>


#ifdef WIN32
#define SNPRINTF _snprintf_s
#else
#define SNPRINTF snprintf
#endif

glm::mat4 GetGlmMat4FromAssimp(aiMatrix4x4 from);

void print_matrix(std::string name, glm::mat4 mat);
