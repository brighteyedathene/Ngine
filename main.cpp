#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>

#include "EngineGlobals.h"
#include "Shader.h"

// Macro for indexing vertex buffer (just forwards the value you give it)
#define BUFFER_OFFSET(i) ((char *)NULL + (i))


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

#pragma endregion callbacks


// From this point on, 'window' refers to the GFLWwindow* window defined in EngineGlobals.h
using namespace ngine;

void Initialise()
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
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	// initialise GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise GLAD" << std::endl;
		exit(EXIT_FAILURE);
	}

	// opengl also needs to know the dimensions of the window
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	// register window for window-resizing callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

}


int main(int argc, char** argv) 
{
	// initialise GLFWwindow and opengl rendering context
	Initialise();

	// create shader program
	Shader shader(vertexShaderPath, fragmentShaderPath);


#pragma region garbage

	// temporary data
	float vertices[] = {
		// posishez          // colourz
		-0.5f, -0.5f, 0.0f,  1.0f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.0f,   1.0f, 1.0f, 0.0f,
		0.0f, 0.5f, 0.0f,    1.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,    1.0f, 0.5f, 0.0f
	};
	unsigned int indices[] = {
		0, 1, 3, // first
		3, 2, 0 // second
	};

	// very simple VAO creation
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);

	/* 
	after this call, all VBO, EBO, glVertexAttribPointer 
	and glEnableVertexAttribArray calls will be stored inside the bound VAO 
	*/
	glBindVertexArray(VAO);

	// veery simple VBO creation
	// fill the VBO with the stupid array of vertices
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// very simple EBO
	// fill the EBO with indices (of 'vertices' array) governing which trios of points make triangles
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// link vertex attributes for VAO
	// point at the positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0); // defined with 'layout(location = 0)' in the vertex shader

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // also layout'd in the vertex shader

	// testing some crap before render loop

	// draw wire frame
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	std::cout << "sizeof(vertices) = " << sizeof(vertices) << std::endl;
	std::cout << "sizeof(vertices)/sizeof(vertices[0]) = " << sizeof(vertices) / sizeof(vertices[0]) << std::endl;

	int n_attributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &n_attributes);
	std::cout << "Maximum number of vertex attributes suppported: " << n_attributes << std::endl;

#pragma endregion garbage

	// render loop!
	while (!glfwWindowShouldClose(window)) 
	{
		// input
		ProcessInput(window);


		// rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader.Use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); // unbind the vertex array


		// swap buffers, check IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete the glfw shit
	glfwTerminate();
	return 0;
}
