#pragma once

#include <GLFW/glfw3.h>

namespace ngine
{
	const int WINDOW_WIDTH = 400; // initial width
	const int WINDOW_HEIGHT = 300; // initial height 

	GLFWwindow* window;


	const GLchar* vertexShaderPath = "./Shaders/shader.vs";
	const GLchar* fragmentShaderPath = "./Shaders/shader.fs";
}