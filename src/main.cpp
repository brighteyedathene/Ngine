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

#include "IKFrame.h"
#include "Spline.h"

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

	Shader invertedHullShader(invertedHullShaderVertexPath, invertedHullShaderFragmentPath);
	Shader lightInvertedHullShader(lightInvertedHullShaderVertexPath, lightInvertedHullShaderFragmentPath);

	Shader colourShader(colourShaderVertexPath, colourShaderFragmentPath);

	Shader subpolyShader(subpolyVertexPath, subpolyFragmentPath);

#pragma region load_assets
	

	// skybox
	Skybox greenSkybox(greenCubemapFaces);

	// Sphere
	Model sphereModel(sphereModelPath);
	NaiveGameObject sphere;
	sphere.SetMesh(&sphereModel);
	sphere.transform.scale = glm::vec3(0.12f);
	sphere.transform.position = glm::vec3(0.7f, 0.0f, 0.0f);

	// Cube
	//Model cubeModel(ikbotPath);
	//NaiveGameObject cube;
	//cube.SetMesh(&cubeModel);
	//cube.transform.position = glm::vec3(0.0f, 0.0f, -10.0f);
	//cube.transform.rotation = glm::vec3(0.0, 0.0, 0.0);
	//cube.transform.scale = glm::vec3(0.05f);

	// Skull
	//Model skullModel(skullPath);

	// Lion
	Model lionModel(lionPath);

	// Missile object
	//Model missileModel(missilePath);
	//NaiveGameObject missile;
	//missile.SetMesh(&missileModel);
	//missile.SetInput(&input);

	// Bear object
	//Model bearModel(bearPath);
	//NaiveGameObject bear;
	//bear.SetMesh(&bearModel);
	//bear.transform.scale = glm::vec3(5.0f);
	//bear.transform.position = glm::vec3(-30.0, 0.0, 0.0);

	// Example object
	NaiveGameObject object;
	object.SetMesh(&lionModel);
	object.SetInput(&input);
	object.transform.scale = glm::vec3(1.0f);
	object.transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
	object.transform.rotation.y = -90.0;

	object.material.ambient = glm::vec3(0.2f, 0.15f, 0.1f);
	object.material.diffuse = glm::vec3(0.9f, 0.7f, 0.5f);
	object.material.specular = glm::vec3(0.8, 0.8f, 0.8f);
	object.material.shininess = 12.0f;

#pragma endregion load_assets


#pragma region camera_init

	Camera mainCamera;
	mainCamera.farClipDistance = 10000;
	mainCamera.nearClipDistance = 0.10f;
	CameraController camController(&mainCamera, &input);
	bool freezecam = false; // TODO move this to camera class						
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// set up camera for shader showcase
	mainCamera.transform.rotation = glm::vec3(0.0, 90.0, 0.0);
	mainCamera.transform.position = glm::vec3(0.0, 0.0, -2.0);

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
	// line drawing
	input.CreateButtonMapping("thicknessMax+", SDL_SCANCODE_KP_4);
	input.CreateButtonMapping("thicknessMax-", SDL_SCANCODE_KP_5);
	float thicknessMax = 0.004;

	input.CreateButtonMapping("thicknessMin+", SDL_SCANCODE_KP_1);
	input.CreateButtonMapping("thicknessMin-", SDL_SCANCODE_KP_2);
	float thicknessMin = 0.002;

	input.CreateButtonMapping("edgeThreshold+", SDL_SCANCODE_KP_7);
	input.CreateButtonMapping("edgeThreshold-", SDL_SCANCODE_KP_8);
	float edgeThreshold = 0.1;

	input.CreateButtonMapping("toggleShaderA", SDL_SCANCODE_KP_9);
	bool shaderAOn = true;
	input.CreateButtonMapping("toggleShaderB", SDL_SCANCODE_KP_6);
	bool shaderBOn = true;

	input.CreateButtonMapping("setUnlitMode", SDL_SCANCODE_7);


	input.CreateButtonMapping("setPenMode", SDL_SCANCODE_1);
	input.CreateButtonMapping("setInkMode", SDL_SCANCODE_2);
	input.CreateButtonMapping("setSuggestiveMode", SDL_SCANCODE_3);
	input.CreateButtonMapping("setThinMode", SDL_SCANCODE_4);
	input.CreateButtonMapping("setInvertedHullOff", SDL_SCANCODE_5);
	input.CreateButtonMapping("setSubpolyOff", SDL_SCANCODE_6);

	input.CreateButtonMapping("toggleDepthTest", SDL_SCANCODE_0);
	bool disableDepthTest = true;

	input.CreateButtonMapping("toggleDebugColours", SDL_SCANCODE_9);
	bool enableDebugColours = false;


	input.CreateButtonMapping("setLightPosition", SDL_SCANCODE_T);


#pragma endregion button_mapping

	float time = 0;

	// Start the clock just before render loop
	//struct Clock clock;
	gameclock.Init();




	// render loop!
	while (!display.IsClosed())
	{
		//display.Clear(0.8f, 0.8f, 0.8f, 1.0f);
		display.Clear(1.0f, 1.0f, 1.0f, 1.0f);

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
			object.transform.position += object.transform.Forward() * 0.01f;
		}
		if (input.GetButton("b_Backward"))
		{
			object.transform.position -= object.transform.Forward() * 0.01f;
		}
		if (input.GetButton("b_YawLeft"))
			object.transform.rotation.y += 1.0f;
		if (input.GetButton("b_YawRight"))
			object.transform.rotation.y -= 1.0f;
		if (input.GetButton("b_PitchUp"))
			object.transform.rotation.x -= 1.0f;
		if (input.GetButton("b_PitchDown"))
			object.transform.rotation.x += 1.0f;
		if (input.GetButton("b_RollLeft"))
			object.transform.rotation.z -= 1.0f;
		if (input.GetButton("b_RollRight"))
			object.transform.rotation.z += 1.0f;


		// Handle Camera controls // TODO move this to the camera controller
		if (input.GetButtonDown("Freeze"))
		{
			freezecam = !freezecam;

			if (freezecam)
			{
				SDL_SetRelativeMouseMode(SDL_FALSE);
				//std::cout << "Camera position: " << mainCamera.transform.ToString() << std::endl;
			}
			else
				SDL_SetRelativeMouseMode(SDL_TRUE);

		}
		if (!freezecam)
			camController.Tick();

		if (input.GetButtonDown("OrthoToggle"))
		{
			mainCamera.orthographic = !mainCamera.orthographic;
		}


#pragma region democontrols

		if (input.GetButton("thicknessMax+"))
		{
			thicknessMax += 0.00005;
			std::cout << "thicknessMax: " << thicknessMax << std::endl;
		}
		if (input.GetButton("thicknessMax-"))
		{
			thicknessMax -= 0.00005;
			std::cout << "thicknessMax: " << thicknessMax << std::endl;
		}

		if (input.GetButton("thicknessMin+"))
		{
			thicknessMin += 0.00005;
			std::cout << "thicknessMin: " << thicknessMin << std::endl;
		}
		if (input.GetButton("thicknessMin-"))
		{
			thicknessMin -= 0.00005;
			std::cout << "thicknessMin: " << thicknessMin << std::endl;
		}

		if (input.GetButton("edgeThreshold+"))
		{
			edgeThreshold += 0.005;
			std::cout << "edgeThreshold: " << edgeThreshold << std::endl;
		}
		if (input.GetButton("edgeThreshold-"))
		{
			edgeThreshold -= 0.005;
			std::cout << "edgeThreshold: " << edgeThreshold << std::endl;
		}

		if (input.GetButtonDown("toggleShaderA"))
		{
			shaderAOn = !shaderAOn;
			std::cout << "inverted hull shader: " << shaderAOn << std::endl;
		}
		if (input.GetButtonDown("toggleShaderB"))
		{
			shaderBOn = !shaderBOn;
			std::cout << "inner shader: " << shaderBOn << std::endl;
		}


		if (input.GetButtonDown("setPenMode"))
		{
			std::cout << "pen" << std::endl;
			edgeThreshold = 0.14;
			thicknessMax = 0.004;
			thicknessMin = 0.0035;
		}
		if (input.GetButtonDown("setInkMode"))
		{
			std::cout << "ink" << std::endl;
			edgeThreshold = 0.36;
			thicknessMax = 0.0029;
			thicknessMin = 0.0026;
		}
		if (input.GetButtonDown("setSuggestiveMode"))
		{
			std::cout << "thick ink" << std::endl;
			edgeThreshold = 0.44;
			thicknessMax = 0.0014;
			thicknessMin = 0.0004;
		}
		if (input.GetButtonDown("setThinMode"))
		{
			std::cout << "thin" << std::endl;
			edgeThreshold = 0.15;
			thicknessMax = 0.0019;
			thicknessMin = 0.0017;
		}
		// kept hitting this one by accident
		//if (input.GetButtonDown("setUnlitMode"))
		//{
		//	std::cout << "unlit" << std::endl;
		//	edgeThreshold = -1.0f;
		//	thicknessMax = 0.037f;
		//	thicknessMin = 0.006f;
		//	shaderAOn = false;
		//}
		if (input.GetButtonDown("setInvertedHullOff"))
		{
			std::cout << "Inverted hull off" << std::endl;
			thicknessMax = 0.0f;
			thicknessMin = 0.0f;
		}
		if (input.GetButtonDown("setSubpolyOff"))
		{
			std::cout << "Subpoly off" << std::endl;
			edgeThreshold = -1.0f;
		}

		if (input.GetButtonDown("toggleDepthTest"))
		{
			disableDepthTest = !disableDepthTest;
			std::cout << "disable depth test: " << disableDepthTest << std::endl;
		}

		if (input.GetButtonDown("toggleDebugColours"))
		{
			enableDebugColours = !enableDebugColours;
			std::cout << "Enable debug colours: " << enableDebugColours << std::endl;
		}

		if (input.GetButtonDown("setLightPosition"))
		{
			sphere.transform.position = mainCamera.transform.position;
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


		
		// Inverted hull stuff
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		if(disableDepthTest)
			glDisable(GL_DEPTH_TEST);

		if (shaderAOn)
		{
			// prepare inverted hull (light)
			lightInvertedHullShader.Use();
			lightInvertedHullShader.SetMat4("projectionview", pv);
			lightInvertedHullShader.SetVec3("viewPos", mainCamera.transform.position);
			lightInvertedHullShader.SetVec3("light.position", sphere.transform.position);
			lightInvertedHullShader.SetVec3("light.colour", glm::vec3(1.0));
			lightInvertedHullShader.SetFloat("edgeThreshold", edgeThreshold);
			lightInvertedHullShader.SetFloat("thicknessMin", thicknessMin);
			lightInvertedHullShader.SetFloat("thicknessMax", thicknessMax);
			if (enableDebugColours)
				lightInvertedHullShader.SetVec3("lineColour", glm::vec3(0.9f, 0.0f, 0.1f));
			else
				lightInvertedHullShader.SetVec3("lineColour", glm::vec3(0.0f));

			object.Draw(&lightInvertedHullShader);
			sphere.Draw(&lightInvertedHullShader);
		}
		else
		{
			// prepare inverted hull shader (colour)
			invertedHullShader.Use();
			invertedHullShader.SetMat4("projectionview", pv);
			invertedHullShader.SetVec3("viewPos", mainCamera.transform.position);
			invertedHullShader.SetFloat("edgeThreshold", edgeThreshold);
			invertedHullShader.SetFloat("thicknessMin", thicknessMin);
			invertedHullShader.SetFloat("thicknessMax", thicknessMax);
			if (enableDebugColours)
				invertedHullShader.SetVec3("lineColour", glm::vec3(0.0f, 0.1f, 0.9f));
			else
				invertedHullShader.SetVec3("lineColour", glm::vec3(0.0f));
			object.Draw(&invertedHullShader);
			sphere.Draw(&invertedHullShader);
		}

		glFrontFace(GL_CCW);
		//glDisable(GL_BLEND);
		//glEnable(GL_CULL_FACE);
		if(disableDepthTest)
			glEnable(GL_DEPTH_TEST);

		// Draw light
		lightShader.Use();
		lightShader.SetMat4("mvp", pv * sphere.transform.GetMatrix());
		lightShader.SetVec3("ucolour", glm::vec3(1.0f));
		sphere.Draw(&lightShader);



		if (shaderBOn)
		{
			// Subpoly shader stuff
			subpolyShader.Use();
			subpolyShader.SetMat4("projectionview", pv);
			subpolyShader.SetVec3("viewPos", mainCamera.transform.position);
			subpolyShader.SetVec3("light.position", sphere.transform.position);
			subpolyShader.SetVec3("light.colour", glm::vec3(1.0));

			subpolyShader.SetFloat("thicknessMin", thicknessMin);
			subpolyShader.SetFloat("thicknessMax", thicknessMax);
			subpolyShader.SetFloat("edgeThreshold", edgeThreshold);
			if (enableDebugColours)
				subpolyShader.SetVec3("lineColour", glm::vec3(0.0f, 0.9f, 0.1f));
			else
				subpolyShader.SetVec3("lineColour", glm::vec3(0.0f));

			object.Draw(&subpolyShader);
			sphere.Draw(&subpolyShader);

		}
		else
		{
			// Show vertex colours
			blinnPhongShader.Use();
			blinnPhongShader.SetMat4("projectionview", pv);
			blinnPhongShader.SetVec3("viewPos", mainCamera.transform.position);

			object.Draw(&blinnPhongShader);
			sphere.Draw(&blinnPhongShader);
		}




		//greenSkybox.Draw(&skyboxShader, &mainCamera);

		glBindVertexArray(0);
		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
