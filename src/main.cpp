#include <glad/glad.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <chrono>

#include "EngineGlobals.h"

#include "Display.h"
#include "EventHandler.h"
#include "Shader.h"
#include "SATexture.h"
#include "Transform.h"
#include "Input.h"
#include "Camera.h"
#include "CameraController.h"
#include "Mesh.h"
#include "Model.h"


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
	std::cout << std::fixed;
	std::cout << std::setprecision(2);
	for (int i = 0; i < 4; i++)
	{
		std::cout << "[ ";
		for (int j = 0; j < 4; j++)
		{
			if (pSource[i+j*4] >= 0)
				std::cout << " ";
			std::cout << pSource[i+j*4] << " ";
		}
		std::cout << "]" << std::endl;
	}
}


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
	Input input;
	EventHandler events(&display, &input);

	// create shader program
	Shader shader(vertexShaderPath, fragmentShaderPath);

	SATexture otherTexture(otherTexturePath);
	SATexture myTexture(texturePath);
	SATexture eye(eyePath);

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

	Model mymodel(modelPath);
	Transform mymodelTransform;
	mymodelTransform.position += 1.0f;
	mymodelTransform.rotation.y = 180;
	mymodelTransform.scale *= 0.5f;

#pragma region camera_init

	Camera mainCamera;
	CameraController camController(&mainCamera, &input);

#pragma endregion camera_init

#pragma region button_mapping

	input.CreateButtonMapping("h", SDL_SCANCODE_H);
	input.CreateButtonMapping("h", SDL_SCANCODE_O);

	input.CreateButtonMapping("Forward", SDL_SCANCODE_W);
	input.CreateButtonMapping("Backward", SDL_SCANCODE_S);
	input.CreateButtonMapping("Left", SDL_SCANCODE_A);
	input.CreateButtonMapping("Right", SDL_SCANCODE_D);

	input.CreateButtonMapping("Escape", SDL_SCANCODE_ESCAPE);

	// putting controls stuff here too - maybe move this later
	SDL_SetRelativeMouseMode(SDL_TRUE);

#pragma endregion button_mapping


	// move the second object around a bit
	transform2.position.x = 0.6f;
	transform2.position.z = 0.1f;
	transform2.position.y = 0.0f;
	transform2.scale = glm::vec3(1.0f) * 0.5f;
	float spin = 0;
	float time = 0;

	// Start the clock just before render loop
	struct Clock clock;


	// render loop!
	while (!display.IsClosed()) 
	{
		display.Clear(0.4f, 0.1f, 0.1f, 1.0f);

		// logic
		if (input.GetButtonDown("Escape"))
		{
			display.Close();
		}

		input.Tick();
		events.Tick();
		clock.Tick();


		// testing input
		bool mybutton = input.GetButton("h");

		if (input.GetButtonDown("h"))
			std::cout << "hi down" << std::endl;
		if (input.GetButtonReleased("h"))
			std::cout << "hi released" << std::endl;

		time += clock.deltaTime;
		float blend = sin(time) / 2 + 0.5f;

		if (spinning)
		{
			transform2.rotation.y = -spin*360;
			transform2.position.y = sin(spin)/2;
			transform2.position.x = 1.5*sin(spin*4);
			transform2.position.z = 1.5*cos(spin * 4);
			spin += clock.deltaTime;
		}
		
		// objects
		camController.Tick();

		// rendering
		shader.Use();
		eye.Bind(0);
		otherTexture.Bind(1);

		glm::mat4 mvp; // more from this later
		glm::mat4 pv = mainCamera.GetViewProjectionMatrix();


		glBindVertexArray(pyrVAO);
		mvp = pv * transform1.GetMatrix();
		shader.SetMat4("mvp", mvp);
		shader.SetFloat("blend", blend);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);
		

		//glBindVertexArray(pyrVAO);
		mvp = pv * transform2.GetMatrix();
		shader.SetMat4("mvp", mvp);
		shader.SetFloat("blend", 1/blend+0.001f);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);

		mymodelTransform.rotation.z = time*190;
		mvp = pv * mymodelTransform.GetMatrix();
		shader.SetMat4("mvp", mvp);
		mymodel.Draw(shader);



		// swap buffers, check IO events, unbind the vertex array
		glBindVertexArray(0);
		

		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
