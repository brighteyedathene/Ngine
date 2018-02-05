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

#include "Skybox.h"

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

	Shader skyboxShader(skyboxVertexPath, skyboxFragmentPath);
	Shader transmittanceShader(transmittanceVertexPath, transmittanceFragmentPath);

	SATexture otherTexture(otherTexturePath);
	SATexture myTexture(texturePath);
	SATexture eye(eyePath);




	// Tell shader where its textures are
	unlitTextureShader.SetInt("texture1", 0); // OR...
	glUniform1i(glGetUniformLocation(unlitTextureShader.ID, "texture2"), 1);
	

	// skybox
	Skybox greenSkybox(greenCubemapFaces);

	// this will be the light
	unsigned int pyrVAO = CreatePyramidVAO();
	int pyrCount = 18;
	Light lightPyramid;
	lightPyramid.colour = glm::vec3(1.0f, 1.0f, 1.0f);
	lightPyramid.transform.scale *= 0.1f;
	lightPyramid.transform.position = glm::vec3(0.0f, 12.5f, -1.0f);


	// Sphere
	Model sphereModel(sphereModelPath);
	NaiveGameObject sphere;
	sphere.SetMesh(&sphereModel);
	sphere.transform.scale = glm::vec3(10.0f);

	// Cube
	Model cubeModel(cubeModelPath);
	NaiveGameObject cube;
	cube.SetMesh(&cubeModel);
	cube.transform.position = glm::vec3(0.0f, 0.0f, 70.0f);

	// Missile object
	Model missileModel(missilePath);
	NaiveGameObject missile;
	missile.SetMesh(&missileModel);
	missile.SetInput(&input);

	// Skull object
	Model skullModel(skullPath);
	NaiveGameObject skull;
	skull.SetMesh(&skullModel);
	skull.SetInput(&input);
	skull.transform.scale = glm::vec3(10.0f);
	skull.transform.position = glm::vec3(0.0f, 0.0f, -70.0f);


	skull.material.ambient = glm::vec3(0.2f, 0.15f, 0.1f);
	skull.material.diffuse = glm::vec3(0.9f, 0.7f, 0.5f);
	skull.material.specular = glm::vec3(0.8, 0.8f, 0.8f);
	skull.material.shininess = 12.0f;


#pragma region camera_init

	Camera mainCamera;
	mainCamera.farClipDistance = 1000;
	CameraController camController(&mainCamera, &input);
	bool freezecam = false; // TODO move this to camera class						
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// set up camera for shader showcase
	mainCamera.transform.rotation = glm::vec3(0.0, 180.0, 0.0);
	mainCamera.transform.position = glm::vec3(142.0, 0.0, 0.0);

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


	// Demo Stuff
	input.CreateButtonMapping("refractiveIndex+", SDL_SCANCODE_1);
	input.CreateButtonMapping("refractiveIndex-", SDL_SCANCODE_2);

	input.CreateButtonMapping("fresnelBias+", SDL_SCANCODE_KP_7);
	input.CreateButtonMapping("fresnelBias-", SDL_SCANCODE_KP_8);
	input.CreateButtonMapping("fresnelScale+", SDL_SCANCODE_KP_4);
	input.CreateButtonMapping("fresnelScale-", SDL_SCANCODE_KP_5);
	input.CreateButtonMapping("fresnelPower+", SDL_SCANCODE_KP_1);
	input.CreateButtonMapping("fresnelPower-", SDL_SCANCODE_KP_2);

	glm::vec3 refractiveIndexRGB = glm::vec3(0.45f, 0.52f, 0.58f);
	float refractiveIndex = 1.52f;
	float fresnelScale = 1.0f;
	float fresnelBias = 0.0f;
	float fresnelPower = 1;

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


		// for now, character movement is handled here...
		if (input.GetButton("b_Forward"))
			cube.transform.position += cube.transform.Forward() * 0.05f;
		if (input.GetButton("b_Backward"))
			cube.transform.position -= cube.transform.Forward() * 0.05f;
		if (input.GetButton("b_YawLeft"))
			cube.transform.rotation.y -= 1.0f;
		if (input.GetButton("b_YawRight"))
			cube.transform.rotation.y += 1.0f;
		if (input.GetButton("b_PitchUp"))
			cube.transform.rotation.x -= 1.0f;
		if (input.GetButton("b_PitchDown"))
			cube.transform.rotation.x += 1.0f;
		if (input.GetButton("b_RollLeft"))
			cube.transform.rotation.z -= 1.0f;
		if (input.GetButton("b_RollRight"))
			cube.transform.rotation.z += 1.0f;
		

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


#pragma region democontrols
		// Demo stuff
		if (input.GetButton("refractiveIndex+"))
		{
			refractiveIndex += 0.02f;
			refractiveIndexRGB.r += 0.01f;
			refractiveIndexRGB.g += 0.01f;
			refractiveIndexRGB.b += 0.01f;
			//std::cout << "refractiveIndex: " << refractiveIndex << std::endl;
			std::cout << "refractiveIndex (RGB): " << refractiveIndexRGB.r << "  " << refractiveIndexRGB.g << "  " << refractiveIndexRGB.b << std::endl;
		}
		if (input.GetButton("refractiveIndex-"))
		{
			refractiveIndex -= 0.02f;
			refractiveIndexRGB.r -= 0.01f;
			refractiveIndexRGB.g -= 0.01f;
			refractiveIndexRGB.b -= 0.01f;
			//std::cout << "refractiveIndex (monochrome): " << refractiveIndex << std::endl;
			std::cout << "refractiveIndex (RGB): " << refractiveIndexRGB.r << "  " << refractiveIndexRGB.g << "  " << refractiveIndexRGB.b << std::endl;
		}
		if (input.GetButton("fresnelScale+"))
		{
			fresnelScale += 0.02f;
			std::cout << "fresnelScale: " << fresnelScale << std::endl;
		}
		if (input.GetButton("fresnelScale-"))
		{
			fresnelScale -= 0.02f;
			std::cout << "fresnelScale: " << fresnelScale << std::endl;
		}
		if (input.GetButton("fresnelBias+"))
		{
			fresnelBias += 0.02f;
			std::cout << "fresnelBias: " << fresnelBias << std::endl;
		}
		if (input.GetButton("fresnelBias-"))
		{
			fresnelBias -= 0.02f;
			std::cout << "fresnelBias: " << fresnelBias << std::endl;
		}
		if (input.GetButton("fresnelPower+"))
		{
			fresnelPower += 0.1;
			std::cout << "fresnelPower: " << fresnelPower << std::endl;
		}
		if (input.GetButton("fresnelPower-"))
		{
			fresnelPower -= 0.1;
			std::cout << "fresnelPower: " << fresnelPower << std::endl;
		}

#pragma endregion democontrols


		// rendering
		eye.Bind(0);
		otherTexture.Bind(1);

		glm::mat4 mvp; // more from this later
		glm::mat4 pv = mainCamera.GetProjectionViewMatrix();


		/*-------------------------------------------------------------------*/
		/*-------------------------- Viewports ------------------------------*/
		/*-------------------------------------------------------------------*/
		int d_width, d_height;
		display.GetWindowSize(&d_width, &d_height);

		glViewport(0, 0, d_width, d_height);


		// prepare transmittance shader
		transmittanceShader.Use();
		transmittanceShader.SetMat4("projectionview", pv);
		transmittanceShader.SetVec3("viewPos", mainCamera.transform.position);
		transmittanceShader.SetInt("skybox", 0);

		transmittanceShader.SetVec3("refractiveIndexRGB", refractiveIndexRGB);
		transmittanceShader.SetFloat("refractiveIndex", refractiveIndex);
		transmittanceShader.SetFloat("scale", fresnelScale);
		transmittanceShader.SetFloat("bias", fresnelBias);
		transmittanceShader.SetFloat("power", fresnelPower);

		transmittanceShader.BindCubemap(greenSkybox.m_cubemap.m_textureID, 0);

		// Skull
		skull.transform.rotation.y += 1.0f;
		skull.Draw(&transmittanceShader);

		// Sphere
		sphere.Draw(&transmittanceShader);
		
		// Cube
		cube.Draw(&transmittanceShader);

		// Draw the skybox
		greenSkybox.Draw(&skyboxShader, &mainCamera);

		glBindVertexArray(0);
		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
