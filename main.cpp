#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>

#include "EngineGlobals.h"


#pragma region shaderz

static const char* vertexShaderSource = "\n\
#version 450\n\
layout (location = 0) in vec3 aPos;\n\
\n\
out vec4 vertexColor;\n\
\n\
void main()\n\
{\n\
	gl_Position = vec4(aPos, 1.0f);\n\
	vertexColor = vec4(0.5, 0.0, 0.0, 1.0);\n\
}\n\
";

static const char* fragmentShaderSource = "\n\
#version 450 core\n\
out vec4 FragColor;\n\
\n\
in vec4 vertexColor;\n\
\n\
void main()\n\
{\n\
	FragColor = vertexColor; \n\
}\n\
";

static const char* dummyFragmentShaderSource = "\n\
#version 450 core\n\
out vec4 FragColor;\n\
uniform vec4 uniColor;\n\
\n\
void main()\n\
{\n\
	FragColor = uniColor; \n\
}\n\
";

static const char* dummyVertexShaderSource = "\n\
#version 450\n\
layout (location = 0) in vec3 aPos;\n\
\n\
void main()\n\
{\n\
	gl_Position = vec4(aPos.x * 0.5f, aPos.y * 0.5f, aPos.z, 1.0f);\n\
}\n\
";

unsigned int CompileShader(const char* shaderSource, GLenum shaderType) 
{
	unsigned int shader;
	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &shaderSource, NULL);
	glCompileShader(shader);
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "Error: Shader compilation failed\n" << infoLog << std::endl;
		return -1;
	}
	return shader;
}

unsigned int LinkShaders(unsigned int shaders[], int numShaders) 
{
	unsigned int shaderProgram = glCreateProgram();
	for (int i = 0; i < numShaders; i++)
	{
		glAttachShader(shaderProgram, shaders[i]);
	}
	glLinkProgram(shaderProgram);
	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "Error: Shader linking failed\n" << infoLog << std::endl;
		return -1;
	}
	return shaderProgram;
}

#pragma endregion shaderz

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

	// compile shaders
	unsigned int vertexShader = CompileShader(vertexShaderSource, GL_VERTEX_SHADER);
	unsigned int fragmentShader = CompileShader(fragmentShaderSource, GL_FRAGMENT_SHADER);

	// create linked shader program
	unsigned int shaders[2] = { vertexShader, fragmentShader };
	unsigned int shaderProgram = LinkShaders(shaders, 2);
	glUseProgram(shaderProgram);

	// TEST create another linked shader
	unsigned int dummyVShader = CompileShader(dummyVertexShaderSource, GL_VERTEX_SHADER);
	unsigned int dummyFShader = CompileShader(dummyFragmentShaderSource, GL_FRAGMENT_SHADER);
	shaders[0] = dummyVShader;
	shaders[1] =  dummyFShader;
	unsigned int dummyShaderProgram = LinkShaders(shaders, 2);


	// delete shaders (they're already in the program)
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


#pragma region garbage

	// temporary data
	float vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); // defined as location 0 in the vertex shader

	// testing some crap before render loop

	// draw wire fram
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


		// logic
		float time = glfwGetTime();
		float green = (sin(time) / 2.0f) + 0.5f;
		int uniColor = glGetUniformLocation(dummyShaderProgram, "uniColor");
		glUseProgram(dummyShaderProgram);
		glUniform4f(uniColor, 0.0f, green, 0.0f, 1.0f);


		// rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); // unbind the vertex array

		glUseProgram(dummyShaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); // unbind the vertex array

		// swap buffers, check events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete the glfw shit
	glfwTerminate();
	return 0;
}
