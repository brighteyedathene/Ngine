#pragma once

#include <GLFW/glfw3.h>

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