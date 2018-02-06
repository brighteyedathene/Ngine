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
	cube.transform.position = glm::vec3(0.0f, -202.0f, 0.0f);
	cube.transform.rotation = glm::vec3(45.0, 0.0, 0.0);
	cube.transform.scale = glm::vec3(15.0f, 0.1f, 60.0f);

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

#pragma endregion load_assets


#pragma region camera_init

	Camera mainCamera;
	mainCamera.farClipDistance = 1000;
	CameraController camController(&mainCamera, &input);
	bool freezecam = false; // TODO move this to camera class						
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// set up camera for shader showcase
	mainCamera.transform.rotation = glm::vec3(0.0, 180.0, 0.0);
	mainCamera.transform.position = glm::vec3(142.0, 30.0, 50.0);

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

	glm::vec3 refractiveIndexRGB = glm::vec3(0.51f, 0.52f, 0.527f);
	float refractiveIndex = 1.52f;
	float fresnelScale = 1.0f;
	float fresnelBias = 0.0f;
	float fresnelPower = 2;

#pragma endregion button_mapping

	float time = 0;

	// Start the clock just before render loop
	//struct Clock clock;
	gameclock.Init();




#pragma region particles

	ParticleSystem particleSystem(
		glm::vec3(0.0, 150.0, 0.0),
		glm::vec3(5.0, 100.0, 5.0),
		10000,
		glm::vec3(20.0, 20.0, 20.0),
		1.0f,
		30.0f
	);

	particleSystem.m_pClock = &gameclock;


	// Random spheres
	int numSpheres = 4;
	vector<glm::vec3> spherePositions;
	vector<float> sphereRadii;

	spherePositions.resize(numSpheres);
	sphereRadii.resize(numSpheres);
	for (int i = 0; i < numSpheres; i++)
	{
		spherePositions[i].x = Randf() * 50.0f;
		spherePositions[i].z = Randf() * 50.0f;
		spherePositions[i].y = Randf() * 200.0f - 100.0f;

		sphereRadii[i] = 3 + Randf() * 15.0f;

		CollisionSphere cs;
		cs.centre = spherePositions[i];
		cs.radius = sphereRadii[i];
		particleSystem.spheres.push_back(cs);
	}

#pragma endregion particles





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
			spherePositions[0].z += 1.0f;
			particleSystem.spheres[0].centre.z += 1.0f;
		}
		if (input.GetButton("b_Backward"))
		{
			spherePositions[0].z -= 1.0f;
			particleSystem.spheres[0].centre.z -= 1.0f;
		}
		if (input.GetButton("b_YawLeft"))
		{
			spherePositions[0].x += 1.0f;
			particleSystem.spheres[0].centre.x += 1.0f;
		}
		if (input.GetButton("b_YawRight"))
		{
			spherePositions[0].x -= 1.0f;
			particleSystem.spheres[0].centre.x -= 1.0f;
		}
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


		glm::mat4 mvp; // more from this later
		glm::mat4 projection = mainCamera.GetProjectionMatrix();
		glm::mat4 view = mainCamera.GetViewMatrix();
		glm::mat4 pv = projection * view;


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
		//skull.Draw(&transmittanceShader);

		// Sphere
		for (int i = 0; i < numSpheres; i++)
		{
			sphere.transform.position = spherePositions[i];
			sphere.transform.scale = glm::vec3(sphereRadii[i]);
			sphere.Draw(&transmittanceShader);
		}
		
		// Cube
		//transmittanceShader.BindCubemap(0, 0);
		cube.Draw(&transmittanceShader);



		// Particle system
		particleShader.Use();
		particleShader.SetMat4("projectionview", pv);
		particleShader.SetVec3("cameraRight", glm::vec3(view[0][0], view[1][0], view[2][0]));
		particleShader.SetVec3("cameraUp", view[0][1], view[1][1], view[2][1]);

		particleSystem.Tick();
		particleSystem.Draw(&particleShader);


		//std::cout << "cam right: " << view[0][0] << " " << view[1][0] << " " << view[2][0] << std::endl;

		// Draw the skybox
		greenSkybox.Draw(&skyboxShader, &mainCamera);

		glBindVertexArray(0);
		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
