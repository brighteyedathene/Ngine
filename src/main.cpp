#include <glad/glad.h>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
//#include <chrono>

#include "EngineGlobals.h"
#include "GameTime.h"
#include "AssortedUtils.h"

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

#include "NaiveGameObject.h"

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

int DEBUG_jointIndex = 0;

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
	Shader animShader(animVertexShaderPath, animFragmentShaderPath);

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

	Model bearModel(bearPath);
	
	Transform mymodelTransform;
	mymodelTransform.position = glm::vec3(3.0f, 1.0f, 2.0f);
	mymodelTransform.rotation.y = 180;
	mymodelTransform.scale *= 0.5f;



	Model bninjaModel(bninjaPath);

	AnimatedModel mixamoModel;
	mixamoModel.LoadMesh(mixamoFBXPATH);

	AnimatedModel ninjaModel;
	ninjaModel.LoadMesh(bninjaPath);

	Animator animator(&ninjaModel);

	NaiveGameObject myCharacter;
	myCharacter.m_pMesh = &ninjaModel;
	myCharacter.transform.scale = glm::vec3(0.01f);

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

	input.CreateButtonMapping("calc0", SDL_SCANCODE_0);
	input.CreateButtonMapping("calc1", SDL_SCANCODE_1);
	input.CreateButtonMapping("calc2", SDL_SCANCODE_2);
	input.CreateButtonMapping("calc3", SDL_SCANCODE_3);

	input.CreateButtonMapping("Freeze", SDL_SCANCODE_F);
	input.CreateButtonMapping("OrthoToggle", SDL_SCANCODE_O);
	bool freezecam = false;

#pragma region camera_init

	Camera mainCamera;
	mainCamera.transform.position.y = 3.0f;
	mainCamera.farClipDistance = 100000;
	CameraController camController(&mainCamera, &input);


	Camera cam4;



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

		if (input.GetButton("b_Forward"))
			myCharacter.transform.position += myCharacter.transform.Forward() * 0.05f;
		if (input.GetButton("b_Backward"))
			myCharacter.transform.position -= myCharacter.transform.Forward() * 0.05f;
		if (input.GetButton("b_Left"))
			myCharacter.transform.rotation.y -= 5.0f;
		if (input.GetButton("b_Right"))
			myCharacter.transform.rotation.y += 5.0f;
		if (input.GetButton("b_Up"))
			myCharacter.transform.rotation.x -= 5.0f;
		if (input.GetButton("b_Down"))
			myCharacter.transform.rotation.x += 5.0f;



		if (input.GetButtonDown("Freeze"))
		{
			freezecam = !freezecam;
			std::cout << "Camera position: " << mainCamera.transform.ToString() << std::endl;
		}
		if(!freezecam)
			camController.Tick();

		if (input.GetButtonDown("OrthoToggle"))
		{
			mainCamera.orthographic = !mainCamera.orthographic;
		}

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

		glm::mat4 pv = mainCamera.GetProjectionViewMatrix();

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
		lightingTestShader.SetMat4("projectionview", pv);

		// all materials are the same for now
		lightingTestShader.SetVec3("material.ambient", 0.1f, 0.1f, 0.4f);
		lightingTestShader.SetVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
		lightingTestShader.SetVec3("material.specular", 0.3, 0.3f, 0.3f);
		lightingTestShader.SetFloat("material.shininess", 10.0f);
		
		// floor
		lightingTestShader.SetVec3("material.ambient", 0.3f, 0.3f, 0.3f);
		mvp = pv * floor.GetMatrix();
		lightingTestShader.SetMat4("model", floor.GetMatrix());
		cube.Draw(lightingTestShader);

		lightingTestShader.SetVec3("material.ambient", 0.1f, 0.1f, 0.4f);
		// rotating cube
		mvp = pv * litCubeTransform.GetMatrix();
		lightingTestShader.SetMat4("model", litCubeTransform.GetMatrix());
		//cube.Draw(lightingTestShader);

		// still drawing the other object so i don't forget how
		//mymodelTransform.rotation.z = time*190;
		mvp = pv * mymodelTransform.GetMatrix();
		lightingTestShader.SetMat4("model", mymodelTransform.GetMatrix());
		//bninjaModel.Draw(lightingTestShader);

		Transform scaler;
		scaler.scale *= 0.001f;
		scaler.position = glm::vec3(0.2f, 0.0f, 0.0f);
		// ANIMATION

		animShader.Use();
		animShader.SetMat4("projectionview", pv);
		animShader.SetVec3("viewPos", mainCamera.transform.position);

		animShader.SetVec3("light.position", lightTransform.position);
		animShader.SetVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		animShader.SetVec3("light.diffuse", 0.5f, 0.5f, 0.5f); // darken the light a bit to fit the scene
		animShader.SetVec3("light.specular", 1.0f, 1.0f, 1.0f);


		animator.Tick(gameclock.deltaTime);
		animShader.SetMat4Array("joints", animator.m_currentMatrices, animator.m_jointCount);
		myCharacter.Draw(&animShader);

		//animShader.SetMat4("model", octobeartran.GetMatrix());
		//ninjaModel.Draw(&animShader);



		



		/*-------------------------------------------------------------------*/
		/*---------------------- CAMERA 4 -----------------------------------*/
		/*-------------------------------------------------------------------*/
		//glViewport(3 * d_width / 4, 0, d_width, d_height / 4);
		glViewport(3 * d_width / 4, 0, d_width / 4, d_height / 4);
		cam4.transform.position = glm::vec3(0.0f, 20.0f, 0.0f);
		cam4.transform.rotation.x = -90.0f;
		cam4.orthoScale = 90;
		glScissor(3 * d_width / 4, 0, d_width / 4, d_height / 4);
		glEnable(GL_SCISSOR_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // (or whatever buffer you want to clear)
		glDisable(GL_SCISSOR_TEST);

		pv = cam4.GetProjectionViewMatrix();
		animShader.SetMat4("projectionview", pv);
		animShader.SetVec3("viewPos", cam4.transform.position);

		myCharacter.Draw(&animShader);

		// Draw floor
		lightingTestShader.Use();
		lightingTestShader.SetVec3("viewPos", cam4.transform.position);
		lightingTestShader.SetMat4("projectionview", pv);
		lightingTestShader.SetMat4("model", floor.GetMatrix());
		cube.Draw(lightingTestShader);


		// Draw the light
		lightShader.Use();
		glBindVertexArray(pyrVAO);
		mvp = pv * lightTransform.GetMatrix();
		lightShader.SetMat4("mvp", mvp);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);



		glBindVertexArray(0);
		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
