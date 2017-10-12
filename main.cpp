#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdlib.h>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include "EngineGlobals.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"
#include "Input.h"

// Macro for indexing vertex buffer (just forwards the value you give it)
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// ...it's global for convenience
Transform transform1;
Transform transform2;
Input input;
bool spinning;

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
	input.Clear();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // otherwise, GLFW_RELEASE
		glfwSetWindowShouldClose(window, true);

	// translation
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		input.f_forward = 1;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		input.f_forward = -1;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		input.f_right = -1;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		input.f_right = 1;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
		input.f_up = 1;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
		input.f_up = -1;

	// Apply translation delta
	transform1.position += glm::vec3(input.f_right*input.translate_delta,
									 input.f_up*input.translate_delta,
									 input.f_forward*input.translate_delta);

	// rotation (affects transform directly)
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		transform1.rotation.x += input.rotate_delta; 
	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		transform1.rotation.y += input.rotate_delta;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		transform1.rotation.z += input.rotate_delta;

	// scale (uniform)
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		transform1.scale += transform1.scale * input.scale_delta;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		transform1.scale -= transform1.scale * input.scale_delta;

	// scale (non-uniform)
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		transform1.scale.y += input.scale_delta;
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		transform1.scale.y -= input.scale_delta;

	// combo
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		spinning = true;
}

#pragma endregion callbacks


#pragma region VAO_creation

unsigned int CreatePyramidVAO()
{
	float vertices[] = {
		// posishez             // colourz           // texture coordz				   					    
		-0.5f, -0.5f, -0.5f,    0.1f, 1.0f, 0.1f,    0.0f, 0.0f, //front left,
		0.5f, -0.5f, -0.5f,     0.1f, 1.0f, 0.1f,    1.0f, 0.0f, // front right
		-0.5f, -0.5f, 0.5f,     0.1f, 0.5f, 0.1f,    0.0f, 1.0f, // back left
		0.5f, -0.5f, 0.5f,      0.1f, 0.5f, 0.1f,    1.0f, 1.0f, // back right
		0.0f, 0.5f, 0.0f,       1.0f, 0.5f, 0.1f,    0.5f, 1.0f // top 
	};
	unsigned int indices[] = {
		// sides
		0,1,4,
		1,3,4,
		3,2,4,
		2,0,4,
		// base
		0,1,3,
		3,2,0
	};

	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0); // defined with 'layout(location = 0)' in the vertex shader
	 // point at the colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // also layout'd in the vertex shader
	// point at the texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return VAO;
}


unsigned int CreateCubeVAO()
{
	// sample data
	float vertices[] = {
		// posishez            // colourz           // texture coordz
		//front				   					    
		-0.5f, -0.5f, 0.5f,    1.0f, 1.0f, 0.0f,    0.0f, 0.0f, //bot left,
		0.5f, -0.5f, 0.5f,     1.0f, 1.0f, 0.0f,    1.0f, 0.0f, // bot right
		-0.5f, 0.5f, 0.5f,     1.0f, 0.5f, 0.0f,    0.0f, 1.0f, // top mid
		0.5f, 0.5f, 0.5f,      1.0f, 0.5f, 0.0f,    1.0f, 1.0f,  // top right

		// back									    
		-0.5f, -0.5f, -0.5f,   1.0f, 1.0f, 0.0f,    0.0f, 0.0f, //bot left,
		0.5f, -0.5f, -0.5f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, // bot right
		-0.5f, 0.5f, -0.5f,    1.0f, 0.5f, 0.0f,    0.0f, 1.0f, // top mid
		0.5f, 0.5f, -0.5f,     1.0f, 0.5f, 0.0f,    1.0f, 1.0f  // top right
	};

	unsigned int indices[] = {
		// front face
		0, 1, 3, // first triangle
		3, 2, 0, // second triangle
		// top face
		2, 3, 6,
		6, 7, 3,
		// left face
		0, 2, 6,
		6, 4, 0,
		// right face
		3, 7, 1,
		1, 5, 7,
		// back face
		5, 4, 6,
		6, 7, 5,
		// bot face
		0, 4, 5,
		5, 1, 0
	};


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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(0));
	glEnableVertexAttribArray(0); // defined with 'layout(location = 0)' in the vertex shader
								  // point at the colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(3 * sizeof(float)));
	glEnableVertexAttribArray(1); // also layout'd in the vertex shader
								  // point at the texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), BUFFER_OFFSET(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	return VAO;
}

#pragma endregion VAO_creation


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

	// may as well do this here
	glEnable(GL_DEPTH_TEST);

	// register window for window-resizing callback function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

}


int main(int argc, char** argv) 
{
	// initialise GLFWwindow and opengl rendering context
	Initialise();

	// create shader program
	Shader shader(vertexShaderPath, fragmentShaderPath);

	Texture otherTexture(otherTexturePath);
	Texture myTexture(texturePath);
	Texture eye(eyePath);

	// Tell shader where its textures are
	shader.Use();
	shader.SetInt("texture1", 0); // OR...
	glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);
	

	unsigned int cubeVAO = CreateCubeVAO();
	int cubeCount = 36;
	unsigned int pyrVAO = CreatePyramidVAO();
	int pyrCount = 18;


	// move the second object around a bit
	transform2.position.x = 0.6f;
	transform2.position.z = 0.1f;
	transform2.position.y = 0.4f;
	transform2.scale = glm::vec3(1.0f) * 0.5f;
	


	// render loop!
	while (!glfwWindowShouldClose(window)) 
	{
		// input
		ProcessInput(window);

		// logic (stupid)
		float time = glfwGetTime();
		float blend = (sin(time)/2) + 0.5f;
		shader.SetFloat("blend", blend);

		if (spinning)
		{
			float spin = (float)((int)time % 360);
			transform2.rotation.y = time;
			transform2.position.y += 0.01f * sin(time);
		}
		

		// rendering
		glClearColor(0.4f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.Use();
		eye.Bind(0);
		otherTexture.Bind(1);
		glBindVertexArray(pyrVAO);

		shader.SetMat4("transform", transform1.GetMatrix());
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);
		
		shader.SetMat4("transform", transform2.GetMatrix());
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);

		// swap buffers, check IO events, unbind the vertex array
		glBindVertexArray(0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// delete the glfw shit
	glfwTerminate();
	return 0;
}
