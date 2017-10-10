#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "EngineGlobals.h"

//using namespace ngine;

#pragma region callbacks

// window-resizing callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
	/* 
	This is a callback function to resize the opengl viewport when the 
	glfw-window size changes 
	*/
	glViewport(0, 0, width, height);
}

void ProcessInput(GLFWwindow* window) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // otherwise, GLFW_RELEASE
		glfwSetWindowShouldClose(window, true);
}

#pragma endregion register_callbacks


using namespace ngine;
int main(int argc, char** argv) 
{
	// initialise GLFW window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window = glfwCreateWindow(
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		"My windwo",
		NULL,
		NULL
	);
	if (window == NULL) 
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// initialise GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		std::cout << "Failed to initialise GLAD" << std::endl;
		return -1;
	}

	// opengl also needs to know the dimensions of the window
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// register window for window-resizing callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// render loop!
	while (!glfwWindowShouldClose(window)) 
	{
		// input
		ProcessInput(window);

		//rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// swap buffers, check events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete the glfw shit
	glfwTerminate();
	return 0;
}
