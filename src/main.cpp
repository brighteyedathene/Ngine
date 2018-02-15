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
#include "ParticleSystem.h"

#include "AnimatedModel.h"
#include "Animator.h"

#include "NaiveGameObject.h"

// ...it's global for convenience
Input input;
bool spinning = true;
float fov = 45.0f;


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

	Shader particleShader(particleShaderVertexPath, particleShaderFragmentPath);
	Shader bumpmapShader(bumpmapShaderVertexPath, bumpmapShaderFragmentPath);


#pragma region load_assets
	

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
	cube.transform.position = glm::vec3(5000.0f, -5000.0f, 5000.0f);
	cube.transform.rotation = glm::vec3(0.0, 0.0, 0.0);
	cube.transform.scale = glm::vec3(500.0f, 500.0f, 500.0f);

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

	SATexture metalDiffuse(metalDiffusePath);
	SATexture metalNormal(metalNormalPath);

	
	SATexture* squaresTexture = new SATexture();
	// This will be loaded later

#pragma endregion load_assets


#pragma region camera_init

	Camera mainCamera;
	mainCamera.farClipDistance = 50000;
	mainCamera.nearClipDistance = 10.0f;
	CameraController camController(&mainCamera, &input);
	bool freezecam = false; // TODO move this to camera class						
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// set up camera for shader showcase
	mainCamera.transform.rotation = glm::vec3(0.0, 45.0, 0.0);
	mainCamera.transform.position = glm::vec3(-50.0, 30.0, -50.0);

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
	input.CreateButtonMapping("min_filter+", SDL_SCANCODE_KP_4);
	input.CreateButtonMapping("min_filter-", SDL_SCANCODE_KP_5);

	input.CreateButtonMapping("mag_filter+", SDL_SCANCODE_KP_7);
	input.CreateButtonMapping("mag_filter-", SDL_SCANCODE_KP_8);

	input.CreateButtonMapping("mip_texture+", SDL_SCANCODE_KP_1);
	input.CreateButtonMapping("mip_texture-", SDL_SCANCODE_KP_2);

	input.CreateButtonMapping("mipmap_toggle", SDL_SCANCODE_M);
	input.CreateButtonMapping("anisotropicFiltering_toggle", SDL_SCANCODE_N);

	bool generateMipMap = false;
	bool anisotropicFiltering = false;
	int minFilterIndex = 0;
	int magFilterIndex = 0;
	int mipTextureIndex = 0;
	
	bool reloadTexture = false;

	vector<GLint> minFilters = {
		GL_NEAREST, GL_LINEAR,
		GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR 
	};
	vector<GLint> magFilters = {
		GL_NEAREST, GL_LINEAR
	};

	vector<const GLchar*> mipPaths = {
		mipPath16,
		mipPath32,
		mipPath64,
		mipPath128,
		mipPath512,
		mipPath1024,
		mipPath2048
	};

	// these labels are ordered to correspond to both filter vectors
	vector<string> filterLabels = {
		"GL_NEAREST (Nearest texel)", "GL_LINEAR (Interpolate texels)",
		"GL_NEAREST_MIPMAP_NEAREST (Nearest texel, nearest mipmap)", "GL_NEAREST_MIPMAP_LINEAR (Nearest texel, interpolate mipmaps)",
		"GL_LINEAR_MIPMAP_NEAREST (Interpolate texels, nearest mipmap)", "GL_LINEAR_MIPMAP_LINEAR (Interpolate texels, interpolate mipmaps)"
	};

	squaresTexture->LoadFromPath(mipPath16, generateMipMap, minFilters[minFilterIndex], magFilters[magFilterIndex], anisotropicFiltering);

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
		{
			cube.transform.position += cube.transform.Forward() * 10.0f;
		}
		if (input.GetButton("b_Backward"))
		{
			cube.transform.position -= cube.transform.Forward() * 10.0f;
		}
		if (input.GetButton("b_YawLeft"))
			cube.transform.rotation.y += 1.0f;
		if (input.GetButton("b_YawRight"))
			cube.transform.rotation.y -= 1.0f;
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
		if (input.GetButtonDown("mipmap_toggle"))
		{
			generateMipMap = !generateMipMap;
			reloadTexture = true;
		}
		if (input.GetButtonDown("anisotropicFiltering_toggle"))
		{
			anisotropicFiltering = !anisotropicFiltering;
			reloadTexture = true;
		}
		if (input.GetButtonDown("min_filter+"))
		{
			minFilterIndex = (minFilterIndex + 1) % minFilters.size();
			reloadTexture = true;
		}
		if (input.GetButtonDown("min_filter-"))
		{
			minFilterIndex = (minFilterIndex - 1 + minFilters.size()) % minFilters.size();
			reloadTexture = true;
		}
		if (input.GetButtonDown("mag_filter+"))
		{
			magFilterIndex = (magFilterIndex + 1) % magFilters.size();
			reloadTexture = true;
		}
		if (input.GetButtonDown("mag_filter-"))
		{
			magFilterIndex = (magFilterIndex - 1) % magFilters.size();
			reloadTexture = true;
		}

		if (input.GetButtonDown("mip_texture+"))
		{
			mipTextureIndex = (mipTextureIndex + 1) % mipPaths.size();
			reloadTexture = true;
		}
		if (input.GetButtonDown("mip_texture-"))
		{
			mipTextureIndex = (mipTextureIndex - 1 + mipPaths.size()) % mipPaths.size();
			reloadTexture = true;
		}

		if (reloadTexture)
		{
			reloadTexture = false;
		std:cout << std::endl;
			std::cout << "Texture number:        " << mipTextureIndex << std::endl;
			std::cout << "Generate mip map:      " << generateMipMap << std::endl;
			std::cout << "Anisotropic filtering: " << anisotropicFiltering << std::endl;
			std::cout << "Min Filter:            " << filterLabels[minFilterIndex] << " (" << minFilterIndex << ")" << std::endl;
			std::cout << "Mag Filter:            " << filterLabels[magFilterIndex] << " (" << magFilterIndex << ")" << std::endl;
			squaresTexture->LoadFromPath(
				mipPaths[mipTextureIndex], 
				generateMipMap,
				minFilters[minFilterIndex], 
				magFilters[magFilterIndex],
				anisotropicFiltering
			);
		}

#pragma endregion democontrols


		// rendering

		glm::mat4 mvp; // more from this later
		glm::mat4 projection = mainCamera.GetProjectionMatrix();
		glm::mat4 view = mainCamera.GetViewMatrix();
		glm::mat4 pv = projection * view;


		int d_width, d_height;
		display.GetWindowSize(&d_width, &d_height);

		glViewport(0, 0, d_width, d_height);

		// prepare transmittance shader
		unlitTextureShader.Use();
		unlitTextureShader.SetMat4("projectionview", pv);
		unlitTextureShader.SetVec3("viewPos", mainCamera.transform.position);

		squaresTexture->Bind(0);
		unlitTextureShader.SetInt("texture1", 0);

		// Cube
		cube.Draw(&unlitTextureShader);

		greenSkybox.Draw(&skyboxShader, &mainCamera);

		glBindVertexArray(0);
		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
