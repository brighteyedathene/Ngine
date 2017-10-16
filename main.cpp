#include <glad/glad.h>

//#include <GLFW/3.h>


#include <iostream>
#include <stdlib.h>
#include <chrono>

//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include "EngineGlobals.h"

#include "Display.h"

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
bool spinning = true;
float fov = 45.0f;


void print_matrix(std::string name, glm::mat4 mat)
{

	float dArray[16] = { 0.0 };

	const float *pSource = (const float*)glm::value_ptr(mat);
	for (int i = 0; i < 16; ++i)
		dArray[i] = pSource[i];



	std::cout << name << ":" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		std::cout << "[ ";
		for (int j = 0; j < 4; j++)
		{
			std::cout << pSource[i+j*4] << " ";
		}
		std::cout << "]" << std::endl;
	}
}

#pragma region callbacks



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


// From this point on, 'display' refers to the Display defined in EngineGlobals.h
using namespace ngine;


#undef main
int main(int argc, char* argv[]) 
{

	Display display(WINDOW_WIDTH, WINDOW_HEIGHT, "My SDL window");



	// create shader program
	Shader shader(vertexShaderPath, fragmentShaderPath);

	Texture otherTexture(otherTexturePath);
	Texture myTexture(texturePath);
	Texture eye(eyePath);

	// Tell shader where its textures are
	shader.Use();
	shader.SetInt("texture1", 0); // OR...
	glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);
	

	// create meshes (garbage section)
	// TODO this but better using member variables and shi
	//unsigned int cubeVAO = CreateCubeVAO();
	//int cubeCount = 36;
	unsigned int pyrVAO = CreatePyramidVAO();
	int pyrCount = 18;


#pragma region camera_init

	glm::mat4 projection;
	projection = glm::perspective(
		glm::radians(fov),
		display.Aspect(),
		0.1f,
		100.0f
	);
	//print_matrix(projection);

	glm::mat4 view;

	glm::mat4 mvp;

#pragma endregion camera_init

#pragma region triangle



#pragma endregion triangle


	// move the second object around a bit
	transform2.position.x = 0.6f;
	transform2.position.z = 0.1f;
	transform2.position.y = 0.0f;
	transform2.scale = glm::vec3(1.0f) * 0.5f;
	float spin = 0;


	// Start the clock just before render loop
	struct Clock clock;

	float time = 0;
	// render loop!
	while (!display.IsClosed()) 
	{
		display.Clear(0.4f, 0.1f, 0.1f, 1.0f);

		// logic (stupid)
		clock.tick();

		time += clock.deltaTime;

		float blend = sin(time) / 2 + 0.5f;

		//std::cout << clock.deltaTime << std::endl;

		shader.SetFloat("blend", blend);
		if (spinning)
		{
			transform2.rotation.y = spin*360;
			transform2.position.y = sin(spin)/2;
			transform2.position.x = sin(spin) / 2;
			spin += clock.deltaTime;
		}
		


		// TODO create Camera object
		projection = glm::perspective(
			glm::radians(fov),
			display.Aspect(),
			0.1f,
			100.0f
		);


		view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));


		// rendering
		shader.Use();
		eye.Bind(0);
		otherTexture.Bind(1);

		glBindVertexArray(pyrVAO);


		glm::mat4 pv = projection * view;
		mvp = pv * transform1.GetMatrix();

		//std::cout << "transform:\n" << transform1.ToString() << std::endl;


		shader.SetMat4("mvp", mvp);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);
		

		glBindVertexArray(pyrVAO);
		mvp = projection * view * transform2.GetMatrix();
		shader.SetMat4("mvp", transform2.GetMatrix());
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);


		// swap buffers, check IO events, unbind the vertex array
		glBindVertexArray(0);
		

		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
