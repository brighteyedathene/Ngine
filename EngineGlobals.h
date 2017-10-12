#pragma once
#ifndef ENGINEGLOBALS_H 
#define ENGINEGLOBALS_H


#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ngine
{
	const int WINDOW_WIDTH = 900; // initial width
	const int WINDOW_HEIGHT = 601; // initial height 

	GLFWwindow* window;


	const GLchar* vertexShaderPath = "./Shaders/shader.vs";
	const GLchar* fragmentShaderPath = "./Shaders/shader.fs";

	const GLchar* texturePath = "./Textures/myTexture.jpg";
	const GLchar* otherTexturePath = "./Textures/otherTexture.jpg";

}

#endif