#pragma once
#ifndef ENGINEGLOBALS_H 
#define ENGINEGLOBALS_H


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ngine
{
	int WINDOW_WIDTH = 900; // initial width
	int WINDOW_HEIGHT = 601; // initial height
	float WINDOW_ASPECT = WINDOW_WIDTH/WINDOW_HEIGHT;

	GLFWwindow* window;


	const GLchar* vertexShaderPath = "./Shaders/shader.vs";
	const GLchar* fragmentShaderPath = "./Shaders/shader.fs";

	const GLchar* texturePath = "./Textures/myTexture.jpg";
	const GLchar* otherTexturePath = "./Textures/otherTexture.jpg";
	const GLchar* eyePath = "./Textures/eye.jpg";

}

#endif