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
	Shader lightingTestShader(lightingTestVertexShaderPath, lightingTestFragmentShaderPath);
	Shader lightShader(lightVertexShaderPath, lightFragmentShaderPath);

	SATexture otherTexture(otherTexturePath);
	SATexture myTexture(texturePath);
	SATexture eye(eyePath);

	// Tell shader where its textures are
	shader.SetInt("texture1", 0); // OR...
	glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);
	

	// create meshes (garbage section)
	// TODO this but better using member variables and shi

	//unsigned int cubeVAO = CreateCubeVAO();
	//int cubeCount = 36;
	

	// this will be the light
	unsigned int pyrVAO = CreatePyramidVAO();
	int pyrCount = 18;
	Transform lightTransform;
	lightTransform.position.x = 0.6f;
	lightTransform.position.z = 0.1f;
	lightTransform.position.y = 0.0f;
	lightTransform.scale = glm::vec3(1.0f) * 0.1f;

	Model cube(cubeModelPath);
	Transform cubeTransform;
	cubeTransform.position.y = 1.0f;
	cubeTransform.rotation.x = 90.0f;
	cubeTransform.scale = glm::vec3(0.01f);
	
	Model bear(bearPath);
	Transform mymodelTransform;
	//mymodelTransform.position = glm::vec3(3.0f, 1.0f, 2.0f);
	mymodelTransform.rotation.y = 180;
	mymodelTransform.scale *= 0.5f;

	Transform litCubeTransform;
	litCubeTransform.scale = glm::vec3(0.03f);
	

#pragma region camera_init

	Camera mainCamera;
	CameraController camController(&mainCamera, &input);

	//mainCamera.orthographic = true;
	//camController.constrainPitch = false;

	Camera cam2;
	cam2.orthographic = true;
	Camera cam3;
	cam3.fov = 120.0f;

	Camera cam4;

	cam2.orthographic = true;


#pragma endregion camera_init

#pragma region button_mapping

	input.CreateButtonMapping("IncreaseSleepTime", SDL_SCANCODE_H);
	input.CreateButtonMapping("DecreaseSleepTime", SDL_SCANCODE_L);

	input.CreateButtonMapping("Forward", SDL_SCANCODE_W);
	input.CreateButtonMapping("Backward", SDL_SCANCODE_S);
	input.CreateButtonMapping("Left", SDL_SCANCODE_A);
	input.CreateButtonMapping("Right", SDL_SCANCODE_D);
	input.CreateButtonMapping("Up", SDL_SCANCODE_Z);
	input.CreateButtonMapping("Down", SDL_SCANCODE_X);

	input.CreateButtonMapping("Escape", SDL_SCANCODE_ESCAPE);

	// putting controls stuff here too - maybe move this later
	SDL_SetRelativeMouseMode(SDL_TRUE);

#pragma endregion button_mapping



	float spin = 0;
	float time = 0;

	// Start the clock just before render loop
	struct Clock clock;

	// render loop!
	while (!display.IsClosed()) 
	{
		display.Clear(0.1f, 0.1f, 0.1f, 1.0f);

		// logic
		if (input.GetButtonDown("Escape"))
		{
			display.Close();
		}

		input.Tick();
		events.Tick();
		clock.Tick();

		time += clock.deltaTime;
		float blend = sin(time) / 2 + 0.5f;

		if (spinning)
		{
			lightTransform.rotation.y += 360*clock.deltaTime;
			lightTransform.position.y = sin(spin)/2;
			lightTransform.position.x = 1.5*sin(spin);
			lightTransform.position.z = 1.5*cos(spin);
			spin += clock.deltaTime;
		}
		
		// update game objects
		camController.Tick();

		// rendering
		eye.Bind(0);
		otherTexture.Bind(1);

		glm::mat4 mvp; // more from this later



		/*-------------------------------------------------------------------*/
		/*-------------------------- Viewports ------------------------------*/
		/*-------------------------------------------------------------------*/
		int d_width, d_height;
		display.GetWindowSize(&d_width, &d_height);
		//std::cout << d_width << "x" << d_height << std::endl;
		/*-------------------------------------------------------------------*/
		/*---------------------- CAMERA 1 -----------------------------------*/
		/*-------------------------------------------------------------------*/
		glViewport(0, d_height / 2, d_width/2, d_height);

		glm::mat4 pv = mainCamera.GetViewProjectionMatrix();

		// Draw the light
		lightShader.Use();
		glBindVertexArray(pyrVAO);
		mvp = pv * lightTransform.GetMatrix();
		lightShader.SetMat4("mvp", mvp);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);


		// Now draw the lit objects
		lightingTestShader.Use();
		lightingTestShader.SetVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		lightingTestShader.SetVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
		lightingTestShader.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);
		lightingTestShader.SetVec3("light.position", lightTransform.position);
		
		lightingTestShader.SetVec3("viewPos", mainCamera.transform.position);

		lightingTestShader.SetVec3("material.ambient", 0.1f, 0.1f, 0.4f);
		lightingTestShader.SetVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		lightingTestShader.SetVec3("material.specular", 0.5, 0.5f, 0.5f);
		lightingTestShader.SetFloat("material.shininess", 60.0f);
		mvp = pv * litCubeTransform.GetMatrix();
		lightingTestShader.SetMat4("mvp", mvp);
		lightingTestShader.SetMat4("model", litCubeTransform.GetMatrix());
		cube.Draw(lightingTestShader);

		// still drawing the other object so i don't forget how
		//mymodelTransform.rotation.z = time*190;
		mvp = pv * mymodelTransform.GetMatrix();
		lightingTestShader.SetMat4("mvp", mvp);
		lightingTestShader.SetMat4("model", mymodelTransform.GetMatrix());
		bear.Draw(lightingTestShader);



		shader.Use();
		cubeTransform.rotation.y += 360 * clock.deltaTime;
		cubeTransform.position.y = 2.0f + sin(time)*0.5f;// * clock.deltaTime;
		mvp = pv* cubeTransform.GetMatrix();
		shader.SetMat4("mvp", mvp);
		cube.Draw(shader);

		// swap buffers, check IO events, unbind the vertex array
		glBindVertexArray(0);
		

		/*-------------------------------------------------------------------*/
		/*---------------------- CAMERA 2 -----------------------------------*/
		/*-------------------------------------------------------------------*/
		glViewport(d_width / 2, d_height / 2, d_width, d_height);
		
		pv = cam2.GetViewProjectionMatrix();
		


		// Draw the light
		lightShader.Use();
		glBindVertexArray(pyrVAO);
		mvp = pv * lightTransform.GetMatrix();
		lightShader.SetMat4("mvp", mvp);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);

		lightingTestShader.Use();
		lightingTestShader.SetVec3("viewPos", cam2.transform.position);

		Transform bear2;
		bear2.rotation.y = time*2;
		bear2.scale *= 0.1f;
		mvp = pv * bear2.GetMatrix();
		lightingTestShader.SetMat4("mvp", mvp);
		lightingTestShader.SetMat4("model", bear2.GetMatrix());
		bear.Draw(lightingTestShader);

		/*-------------------------------------------------------------------*/
		/*---------------------- CAMERA 3 -----------------------------------*/
		/*-------------------------------------------------------------------*/
		glViewport(0, 0, d_width / 2, d_height / 2);

		pv = cam3.GetViewProjectionMatrix();

		Transform staticLight;
		staticLight.position = glm::vec3(0, 2, -3);
		// Draw the light
		lightShader.Use();
		glBindVertexArray(pyrVAO);
		mvp = pv * staticLight.GetMatrix();
		lightShader.SetMat4("mvp", mvp);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);

		lightingTestShader.Use();
		lightingTestShader.SetVec3("viewPos", cam3.transform.position);
		lightingTestShader.SetVec3("light.position", staticLight.position);

		Transform bear3;
		//printf("Z: %13.8f\n", time);
		bear3.position.z = 1.0f - sin(time)*2.0f;
		bear3.rotation.y = 180.0f;
		mvp = pv * bear3.GetMatrix();
		lightingTestShader.SetMat4("mvp", mvp);
		lightingTestShader.SetMat4("model", bear3.GetMatrix());
		bear.Draw(lightingTestShader);


		/*-------------------------------------------------------------------*/
		/*---------------------- CAMERA 4 -----------------------------------*/
		/*-------------------------------------------------------------------*/
		glViewport(d_width / 2, 0, d_width, d_height / 2);
		cam4.transform.position = glm::vec3(-2.0f, 0.0f, -3.0f);
		//std::cout << cam4.transform.ToString() << std::endl;

		pv = cam4.GetViewProjectionMatrix();

		staticLight.position = glm::vec3(0, 0, 0);
		// Draw the light
		lightShader.Use();
		glBindVertexArray(pyrVAO);
		mvp = pv * staticLight.GetMatrix();
		lightShader.SetMat4("mvp", mvp);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);
		
		lightingTestShader.Use();
		lightingTestShader.SetVec3("viewPos", cam4.transform.position);
		lightingTestShader.SetVec3("light.position", staticLight.position);
		// draw bears
		for (int i = 0; i < 10; i++)
		{
			Transform b;
			b.scale *= 0.1f;
			b.position.y = sin((time + i*2) * 2);
			b.position.x = cos((time + i*2) * 2);
			
			b.rotation.y = 180.0f;
			b.rotation.z = i * 360 / 10;

			mvp = pv * b.GetMatrix();
			lightingTestShader.SetMat4("mvp", mvp);
			lightingTestShader.SetMat4("model", b.GetMatrix());
			bear.Draw(lightingTestShader);

		}

		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
