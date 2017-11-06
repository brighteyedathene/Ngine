#include <glad/glad.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
//#include <chrono>

#include "EngineGlobals.h"
#include "GameTime.h"

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

#include "AnimatedModel.h"
#include "Animator.h"

// Macro for indexing vertex buffer (just forwards the value you give it)
#define BUFFER_OFFSET(i) ((char *)NULL + (i))

// ...it's global for convenience
Input input;
bool spinning = true;
float fov = 45.0f;


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
	
	Transform floor;
	floor.scale = glm::vec3(20.0f, 0.001f, 20.0f);
	floor.position.y = -1.0f;

	Model bearModel(bearPath);
	
	Transform mymodelTransform;
	mymodelTransform.position = glm::vec3(3.0f, 1.0f, 2.0f);
	mymodelTransform.rotation.y = 180;
	mymodelTransform.scale *= 0.5f;



	Model bninjaModel(bninjaPath);
	AnimatedModel animModel;
	animModel.LoadMesh(bninjaPath);
	Animator animator(&animModel);

	Transform litCubeTransform;
	litCubeTransform.scale = glm::vec3(0.03f);
	


	// Moveing the bear forwards
	// Why are these 90.0f offsets necessary?

	Transform octobeartran;
	octobeartran.rotation.y = -90.0f;
	//octobeartran.rotation.x = -90.0f;
	octobeartran.scale = glm::vec3(0.1f);

	input.CreateButtonMapping("b_Forward", SDL_SCANCODE_I);
	input.CreateButtonMapping("b_Backward", SDL_SCANCODE_K);
	input.CreateButtonMapping("b_Left", SDL_SCANCODE_J);
	input.CreateButtonMapping("b_Right", SDL_SCANCODE_L);
	input.CreateButtonMapping("b_Up", SDL_SCANCODE_Y);
	input.CreateButtonMapping("b_Down", SDL_SCANCODE_H);

	input.CreateButtonMapping("Freeze", SDL_SCANCODE_F);
	bool freezecam = false;

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
	//struct Clock clock;
	gameclock.Init();


	// render loop!
	while (!display.IsClosed()) 
	{
		display.Clear(0.0f, 0.0f, 0.0f, 1.0f);

		// logic
		if (input.GetButtonDown("Escape"))
		{
			display.Close();
		}

		input.Tick();
		events.Tick();
		gameclock.Tick();


		time = gameclock.time;
		float blend = sin(time) / 2 + 0.5f;

		if (spinning)
		{
			lightTransform.rotation.y += 360* gameclock.deltaTime;
			lightTransform.position.y = 1 + sin(spin)/2;
			lightTransform.position.x = 1.5*sin(spin);
			lightTransform.position.z = 1.5*cos(spin);
			spin += gameclock.deltaTime;
		}
		
		// update game objects
		if (input.GetButtonDown("Freeze"))
		{
			freezecam = !freezecam;
		}
		if(!freezecam)
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
		/*-------------------------------------------------------------------*/
		/*---------------------- CAMERA 1 -----------------------------------*/
		/*-------------------------------------------------------------------*/
		glViewport(0, 0, d_width, d_height);

		glPolygonMode(GL_FRONT, GL_FILL);

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

		// all materials are the same for now
		lightingTestShader.SetVec3("material.ambient", 0.1f, 0.1f, 0.4f);
		lightingTestShader.SetVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		lightingTestShader.SetVec3("material.specular", 0.3, 0.3f, 0.3f);
		lightingTestShader.SetFloat("material.shininess", 10.0f);
		
		// floor
		lightingTestShader.SetVec3("material.ambient", 0.3f, 0.3f, 0.3f);
		mvp = pv * floor.GetMatrix();
		lightingTestShader.SetMat4("mvp", mvp);
		lightingTestShader.SetMat4("model", floor.GetMatrix());
		//cube.Draw(lightingTestShader);

		lightingTestShader.SetVec3("material.ambient", 0.1f, 0.1f, 0.4f);
		// rotating cube
		mvp = pv * litCubeTransform.GetMatrix();
		lightingTestShader.SetMat4("mvp", mvp);
		lightingTestShader.SetMat4("model", litCubeTransform.GetMatrix());
		//cube.Draw(lightingTestShader);

		// still drawing the other object so i don't forget how
		//mymodelTransform.rotation.z = time*190;
		mvp = pv * mymodelTransform.GetMatrix();
		lightingTestShader.SetMat4("mvp", mvp);
		lightingTestShader.SetMat4("model", mymodelTransform.GetMatrix());
		//bninjaModel.Draw(lightingTestShader);

		Transform scaler;
		scaler.scale *= 0.01f;

		// investigating skellies
		lightingTestShader.SetVec3("material.ambient", 1, 1, 1);
		lightingTestShader.SetVec3("material.diffuse", 1, 1, 1);
		lightingTestShader.SetVec3("material.specular", 1, 1, 1);


		for (int i = 0; i < animModel.m_Skeleton.m_jointCount; i++)
		{
			glm::mat4 modmat = animModel.m_Skeleton.GetModelSpaceBindMatrix(i);

			

			modmat = modmat * scaler.GetMatrix();
			lightingTestShader.SetMat4("mvp", pv * modmat);
			lightingTestShader.SetMat4("model", modmat);
			cube.Draw(lightingTestShader);
		}

		
		if (input.GetButton("b_Forward"))
			octobeartran.position += octobeartran.Forward() * 0.01f;
		if (input.GetButton("b_Backward"))
			octobeartran.position -= octobeartran.Forward() * 0.01f;
		if (input.GetButton("b_Left"))
			octobeartran.rotation.y -= 5.0f;
		if (input.GetButton("b_Right"))
			octobeartran.rotation.y += 5.0f;
		if (input.GetButton("b_Up"))
			octobeartran.rotation.x -= 5.0f;
		if (input.GetButton("b_Down"))
			octobeartran.rotation.x += 5.0f;

		Transform nin;
		nin.position = octobeartran.position;
		nin.rotation = octobeartran.rotation;
		nin.scale = octobeartran.scale;
		
		nin.rotation.y = -octobeartran.rotation.y + 90.0f;
		nin.rotation.x = -octobeartran.rotation.x;// +90.0f;


		lightingTestShader.SetMat4("mvp", pv * nin.GetMatrix());
		lightingTestShader.SetMat4("model", nin.GetMatrix());
		bearModel.Draw(lightingTestShader);

		
		
		// smiley cube
		//shader.Use();
		//cubeTransform.rotation.y += 360 * gameclock.deltaTime;
		//cubeTransform.position.y = 2.0f + sin(time)*0.5f;// * clock.deltaTime;
		//mvp = pv* cubeTransform.GetMatrix();
		//shader.SetMat4("mvp", mvp);
		//cube.Draw(shader);

		// swap buffers, check IO events, unbind the vertex array
		glBindVertexArray(0);
		



		/*-------------------------------------------------------------------*/
		/*---------------------- CAMERA 4 -----------------------------------*/
		/*-------------------------------------------------------------------*/
		glViewport(7 * d_width / 8, 0, d_width, d_height / 8);
		cam4.transform.position = glm::vec3(-2.0f, 0.0f, -3.0f);
		cam4.fov = 100;
		//glScissor( d_width / 2, 0, d_width, d_height / 2);
		//glEnable(GL_SCISSOR_TEST);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // (or whatever buffer you want to clear)
		//glDisable(GL_SCISSOR_TEST);

		pv = cam4.GetViewProjectionMatrix();
		
		Transform staticLight;
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
			bearModel.Draw(lightingTestShader);

		}

		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
