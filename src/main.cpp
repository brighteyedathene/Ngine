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
#include "Light.h"

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


// namespace ngine contains all the paths for assets and some default values for the window
using namespace ngine;

int DEBUG_jointIndex = 0;

#undef main
int main(int argc, char* argv[]) 
{
	Display display(WINDOW_WIDTH, WINDOW_HEIGHT, "My SDL window");
	Input input;
	EventHandler events(&display, &input);

	// create shader program
	Shader unlitTextureShader(vertexShaderPath, fragmentShaderPath);
	Shader lightingTestShader(lightingTestVertexShaderPath, lightingTestFragmentShaderPath);
	Shader lightShader(lightVertexShaderPath, lightFragmentShaderPath);
	Shader animShader(animVertexShaderPath, animFragmentShaderPath);
	
	Shader flatShader(flatShaderVertexPath, flatShaderFragmentPath);
	Shader blinnPhongShader(blinnPhongShaderVertexPath, blinPhongShaderFragmentPath);
	Shader minnaertShader(minnaertShaderVertexPath, minnaertShaderFragmentPath);

	SATexture otherTexture(otherTexturePath);
	SATexture myTexture(texturePath);
	SATexture eye(eyePath);

	// Tell shader where its textures are
	unlitTextureShader.SetInt("texture1", 0); // OR...
	glUniform1i(glGetUniformLocation(unlitTextureShader.ID, "texture2"), 1);
	

	// this will be the light
	unsigned int pyrVAO = CreatePyramidVAO();
	int pyrCount = 18;
	Light lightPyramid;
	lightPyramid.colour = glm::vec3(1.0f, 1.0f, 1.0f);
	lightPyramid.transform.scale *= 0.1f;
	lightPyramid.transform.position = glm::vec3(0.0f, 12.5f, -1.0f);


	// Floor object
	Model cubeModel(cubeModelPath);
	NaiveGameObject floor;
	floor.SetMesh(&cubeModel);
	floor.transform.scale = glm::vec3(20.0f, 0.001f, 20.0f);

	floor.material.ambient = glm::vec3( 0.0f, 0.0f, 0.0f);
	floor.material.diffuse = glm::vec3( 1.0f, 0.5f, 0.31f);
	floor.material.specular = glm::vec3( 0.0, 0.0f, 0.0f);
	floor.material.shininess = 30.0f;


	// Missile object
	Model missileModel(missilePath);
	NaiveGameObject missile;
	missile.SetMesh(&missileModel);
	missile.SetInput(&input);
	//missile.transform.scale = glm::vec3(0.1f);
	missile.transform.position.y = 20.0f;

	missile.material.ambient = glm::vec3(0.2f, 0.2f, 0.3f);
	missile.material.diffuse = glm::vec3(0.4f, 0.45f, 0.6f);
	missile.material.specular = glm::vec3(0.8, 0.8f, 0.8f);
	missile.material.shininess = 12.0f;

	// Major Kong

	AnimatedModel controlModel;
	controlModel.LoadMesh(bninjaPath);

	AnimatedModel majorkongModel;
	majorkongModel.LoadMesh(majorkongPath);
	Animator animator(&majorkongModel);
	NaiveGameObject majorkong;
	majorkong.SetMesh(&majorkongModel);
	majorkong.SetInput(&input);
	majorkong.transform.scale = glm::vec3(0.01f);

	// Skull object
	Model skullModel(skullPath);
	NaiveGameObject skull;
	skull.SetMesh(&skullModel);
	skull.SetInput(&input);
	skull.transform.scale = glm::vec3(3.5f);


	skull.material.ambient = glm::vec3(0.2f, 0.15f, 0.1f);
	skull.material.diffuse = glm::vec3(0.9f, 0.7f, 0.5f);
	skull.material.specular = glm::vec3(0.8, 0.8f, 0.8f);
	skull.material.shininess = 12.0f;

	// Vector of skull positions
	int NUM_SKULLS = 30;
	float RANGE = 200;
	vector<Transform> skullTransforms;
	skullTransforms.resize(NUM_SKULLS);
	for (int i = 0; i < NUM_SKULLS; i++) {
		float random_x = (RANGE * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - RANGE / 2;
		float random_z = (RANGE * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) - RANGE / 2;
		skullTransforms[i].position.x = random_x;
		skullTransforms[i].position.z = random_z;
		skullTransforms[i].position.y = 4.0f;
		skullTransforms[i].rotation.y = 360 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	}

#pragma region camera_init

	Camera mainCamera;
	mainCamera.farClipDistance = 1000;
	CameraController camController(&mainCamera, &input);
	bool freezecam = false; // TODO move this to camera class						
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// set up camera for shader showcase
	mainCamera.transform.rotation = glm::vec3(-20.0, -45.0, 0.0);
	mainCamera.transform.position = glm::vec3(-13.0, 10.0, 25.0);

#pragma endregion camera_init



#pragma region button_mapping

	input.CreateButtonMapping("Escape", SDL_SCANCODE_ESCAPE);

	// Camera controls
	input.CreateButtonMapping("Forward", SDL_SCANCODE_W);
	input.CreateButtonMapping("Backward", SDL_SCANCODE_S);
	input.CreateButtonMapping("Left", SDL_SCANCODE_A);
	input.CreateButtonMapping("Right", SDL_SCANCODE_D);
	input.CreateButtonMapping("Up", SDL_SCANCODE_Z);
	input.CreateButtonMapping("Down", SDL_SCANCODE_X);

	input.CreateButtonMapping("Freeze", SDL_SCANCODE_F);
	input.CreateButtonMapping("OrthoToggle", SDL_SCANCODE_P);


	// Controls for moving the model directly
	input.CreateButtonMapping("b_Forward", SDL_SCANCODE_I);
	input.CreateButtonMapping("b_Backward", SDL_SCANCODE_K);
	input.CreateButtonMapping("b_YawLeft", SDL_SCANCODE_J);
	input.CreateButtonMapping("b_YawRight", SDL_SCANCODE_L);
	input.CreateButtonMapping("b_PitchUp", SDL_SCANCODE_Y);
	input.CreateButtonMapping("b_PitchDown", SDL_SCANCODE_H);
	input.CreateButtonMapping("b_RollLeft", SDL_SCANCODE_U);
	input.CreateButtonMapping("b_RollRight", SDL_SCANCODE_O);



	// Lab specific controls
	input.CreateButtonMapping("toggleUseQuat", SDL_SCANCODE_T);
	input.CreateButtonMapping("toggleFirstPerson", SDL_SCANCODE_R);
	bool useQuat = false;
	bool firstPerson = false;
	QTransform qtransform;
	qtransform.position = missile.transform.position;
	qtransform.rotation = EulerToQuat(missile.transform.rotation);
	qtransform.scale = missile.transform.scale;


#pragma endregion button_mapping

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


		// Toggle fpp, useQuat
		if (input.GetButtonDown("toggleUseQuat"))
			useQuat = !useQuat;
		if (input.GetButtonDown("toggleFirstPerson"))
			firstPerson = !firstPerson;


		// for now, character movement is handled here...
		if (input.GetButton("b_Forward"))
			missile.transform.position += missile.transform.Forward() * 0.05f;
		if (input.GetButton("b_Backward"))
			missile.transform.position -= missile.transform.Forward() * 0.05f;
		// rotations
		if (!useQuat) 
		{
			if (input.GetButton("b_YawLeft"))
				missile.transform.rotation.y -= 1.0f;
			if (input.GetButton("b_YawRight"))
				missile.transform.rotation.y += 1.0f;
			if (input.GetButton("b_PitchUp"))
				missile.transform.rotation.x -= 1.0f;
			if (input.GetButton("b_PitchDown"))
				missile.transform.rotation.x += 1.0f;
			if (input.GetButton("b_RollLeft"))
				missile.transform.rotation.z -= 1.0f;
			if (input.GetButton("b_RollRight"))
				missile.transform.rotation.z += 1.0f;
		}
		else 
		{
			// Use quaternions
			glm::vec3 eulerRotation = glm::vec3(0.0f);
			if (input.GetButton("b_YawLeft"))
				eulerRotation.y -= 1.0f;
			if (input.GetButton("b_YawRight"))
				eulerRotation.y += 1.0f;
			if (input.GetButton("b_PitchUp"))
				eulerRotation.x -= 1.0f;
			if (input.GetButton("b_PitchDown"))
				eulerRotation.x += 1.0f;
			if (input.GetButton("b_RollLeft"))
				eulerRotation.z -= 1.0f;
			if (input.GetButton("b_RollRight"))
				eulerRotation.z += 1.0f;

			glm::quat deltaq = EulerToQuat(eulerRotation);
			//glm::quat q1 = EulerToQuat(missile.transform.rotation);
			glm::quat q2 = qtransform.rotation * deltaq;
			q2 = glm::normalize(q2);
			std::cout << "qtransform quat: " << qtransform.rotation.w <<" "<< qtransform.rotation.x <<" "<< qtransform.rotation.y <<" "<< qtransform.rotation.z << std::endl;
			qtransform.rotation = q2;
			glm::vec3 newEulerRotation = glm::eulerAngles(qtransform.rotation);
			missile.transform.rotation = newEulerRotation;
			
		}

		// Handle Camera controls // TODO move this to the camera controller
		if (input.GetButtonDown("Freeze"))
		{
			freezecam = !freezecam;

			if (freezecam) 
			{
				SDL_SetRelativeMouseMode(SDL_FALSE);
				std::cout << "Camera position: " << mainCamera.transform.ToString() << std::endl;
			}
			else
				SDL_SetRelativeMouseMode(SDL_TRUE);

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
		glm::mat4 pv = mainCamera.GetProjectionViewMatrix();

		// Apply first person constraint (basically remove all controls)
		if (firstPerson) {
			mainCamera.transform.position = missile.transform.position + missile.transform.Up()*1.5f;
			mainCamera.transform.rotation = missile.transform.rotation;
			//mainCamera.transform.rotation = glm::eulerAngles(qtransform.rotation);
			//glm::quat missileQuat = glm::quat_cast(missile.GetOffsetTransformMatrix());
			//mainCamera.transform.rotation = glm::eulerAngles(missileQuat);

			//glm::mat4 fpp = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -4.5f, 0.0f));
			//fpp = fpp* missile.transform.GetMatrix();
			//pv = mainCamera.GetProjectionViewMatrix(fpp);
		}

		/*-------------------------------------------------------------------*/
		/*-------------------------- Viewports ------------------------------*/
		/*-------------------------------------------------------------------*/
		int d_width, d_height;
		display.GetWindowSize(&d_width, &d_height);

		glViewport(0, 0, d_width, d_height);

		// Draw the light
		lightShader.Use();
		glBindVertexArray(pyrVAO);
		mvp = pv * lightPyramid.transform.GetMatrix();
		lightShader.SetMat4("mvp", mvp);
		lightShader.SetVec3("ucolour", lightPyramid.colour);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);


		// Now draw the lit objects
		flatShader.Use();
		flatShader.SetInt("numShades", 2);

		flatShader.SetVec3("light.position", lightPyramid.transform.position);
		flatShader.SetVec3("light.colour",  lightPyramid.colour);
		flatShader.SetVec3("viewPos", mainCamera.transform.position);
		flatShader.SetMat4("projectionview", pv);

		// floor
		floor.Draw(&flatShader);

		// skulls
		for (int i = 0; i < NUM_SKULLS; i++) {
			skull.transform.position = skullTransforms[i].position;
			skull.transform.rotation = skullTransforms[i].rotation;
			skullTransforms[i].rotation.y += 1.0f;
			skull.Draw(&flatShader);
		}

		// missile
		missile.material.specular = glm::vec3(1.0f);
		missile.material.shininess = 12.0f;
		missile.Draw(&flatShader);

		// major kong
		animShader.Use();
		animShader.SetMat4("projectionview", pv);
		animShader.SetVec3("viewPos", mainCamera.transform.position);
		animShader.SetVec3("light.position", lightPyramid.transform.position);
		//animShader.SetVec3("light.colour", lightPyramid.colour);
		animShader.SetVec3("light.ambient", glm::vec3(0.6f));
		animShader.SetVec3("light.diffuse", lightPyramid.colour);
		animShader.SetVec3("light.specular", lightPyramid.colour);
		animator.Tick(gameclock.deltaTime);
		animShader.SetMat4Array("joints", animator.m_currentMatrices, animator.m_jointCount);
		
		//majorkong.Draw(&animShader); // instead of this, do the following:
		animShader.SetMat4("model", missile.GetOffsetTransformMatrix() * majorkong.transform.GetMatrix()); // the parent's model matrix
		majorkongModel.Draw(&animShader); // Draw directly from animated model - not the gameobject


		glBindVertexArray(0);
		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
