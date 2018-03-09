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

	Shader colourShader(colourShaderVertexPath, colourShaderFragmentPath);

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
	sphere.transform.scale = glm::vec3(0.12f);
	sphere.transform.position = glm::vec3(0.0);

	NaiveGameObject goalmarker;
	goalmarker.SetMesh(&sphereModel);
	goalmarker.transform.scale = glm::vec3(0.12f);
	goalmarker.transform.position = glm::vec3(0.0);

	// Cube
	Model cubeModel(cubeModelPath);
	NaiveGameObject cube;
	cube.SetMesh(&cubeModel);
	cube.transform.position = glm::vec3(0.0f, 0.0f, -10.0f);
	cube.transform.rotation = glm::vec3(0.0, 0.0, 0.0);
	cube.transform.scale = glm::vec3(0.05f);

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
	skull.transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
	skull.transform.rotation.y = -90.0;

	skull.material.ambient = glm::vec3(0.2f, 0.15f, 0.1f);
	skull.material.diffuse = glm::vec3(0.9f, 0.7f, 0.5f);
	skull.material.specular = glm::vec3(0.8, 0.8f, 0.8f);
	skull.material.shininess = 12.0f;

	// Bear object
	Model bearModel(bearPath);
	NaiveGameObject bear;
	bear.SetMesh(&bearModel);
	bear.transform.scale = glm::vec3(5.0f);
	bear.transform.position = glm::vec3(-30.0, 0.0, 0.0);

	// IK
	Model ikjointModel(ikjointPath);
	IKFrame ikframe(3);
	ikframe.jointModel = &ikjointModel;
	ikframe.transform.position = glm::vec3(0.0, 0.0, 0.0);
	ikframe.goal = glm::vec3(1.0, 1.0, 1.0);

	ikframe.joints[2].length *= 0.3f;

	ikframe.joints[1].dof.down = 0.0f;
	//ikframe.joints[1].dof.left = 1.0f;
	ikframe.joints[1].dof.right = 0.0f;
	//ikframe.joints[0].dof.down = 100.0f;
	//ikframe.joints[0].dof.up = 120.0f;

	ikframe.Init();


	// trunk
	IKFrame iktrunk(20);
	iktrunk.jointModel = &ikjointModel;
	iktrunk.transform.position = glm::vec3(6.0, 0.0, 0.0);
	for (int i = 0; i < iktrunk.joints.size(); i++)
	{
		iktrunk.joints[i].length = 0.5f;
	}
	iktrunk.Init();

	// Spline
	Spline spline(3);
	spline.model = &sphereModel;
	spline.controlPoints[3] = glm::vec3(0.0, 3.0, 3.0);
	spline.controlPoints[4] = glm::vec3(1.0, 3.0, 3.0);
	spline.controlPoints[5] = glm::vec3(2.0, 5.0, 3.0);
	spline.Smoothen();

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
	float thicknessMax = 0.05;

	input.CreateButtonMapping("thicknessMin+", SDL_SCANCODE_KP_1);
	input.CreateButtonMapping("thicknessMin-", SDL_SCANCODE_KP_2);
	float thicknessMin = 0.01;

	input.CreateButtonMapping("mag_filter+", SDL_SCANCODE_KP_7);
	input.CreateButtonMapping("mag_filter-", SDL_SCANCODE_KP_8);

	input.CreateButtonMapping("mip_texture+", SDL_SCANCODE_KP_1);
	input.CreateButtonMapping("mip_texture-", SDL_SCANCODE_KP_2);

	input.CreateButtonMapping("mipmap_toggle", SDL_SCANCODE_M);
	input.CreateButtonMapping("anisotropicFiltering_toggle", SDL_SCANCODE_N);

	// ik
	input.CreateButtonMapping("toggleMoveIKFrameWithCamera", SDL_SCANCODE_T);
	bool moveIKFrameWithCamera = false;
	 
	input.CreateButtonMapping("toggleConstraints", SDL_SCANCODE_C);
	bool constrainIK = false;
	ikframe.constrain = constrainIK;
	iktrunk.constrain = constrainIK;

#pragma endregion button_mapping

	float time = 0;

	// Start the clock just before render loop
	//struct Clock clock;
	gameclock.Init();




	// render loop!
	while (!display.IsClosed()) 
	{
		display.Clear(0.8f, 0.8f, 0.8f, 1.0f);

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
			cube.transform.position += cube.transform.Forward() * 0.01f;
		}
		if (input.GetButton("b_Backward"))
		{
			cube.transform.position -= cube.transform.Forward() * 0.01f;
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
				//std::cout << "Camera position: " << mainCamera.transform.ToString() << std::endl;
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

		if (input.GetButton("thicknessMax+"))
		{
			thicknessMax += 0.0001;
			std::cout << "thicknessMax: " << thicknessMax << std::endl;
		}
		if (input.GetButton("thicknessMax-"))
		{
			thicknessMax -= 0.0001;
			std::cout << "thicknessMax: " << thicknessMax << std::endl;
		}

		if (input.GetButton("thicknessMin+"))
		{
			thicknessMin += 0.0001;
			std::cout << "thicknessMin: " << thicknessMin << std::endl;
		}
		if (input.GetButton("thicknessMin-"))
		{
			thicknessMin -= 0.0001;
			std::cout << "thicknessMin: " << thicknessMin << std::endl;
		}

		if (input.GetButtonDown("toggleMoveIKFrameWithCamera"))
		{
			moveIKFrameWithCamera = !moveIKFrameWithCamera;
		}
		if (input.GetButtonDown("toggleConstraints"))
		{
			constrainIK = !constrainIK;
			ikframe.constrain = constrainIK;
			iktrunk.constrain = constrainIK;
			std::cout << "constrain joints: " << constrainIK << std::endl;
		}

#pragma endregion democontrols

		// ik pass
		float deviation = sin(time/4) / 2 + 0.5;
		//deviation = 0.4f;
		ikframe.goal = spline.Sample(deviation);

		if (moveIKFrameWithCamera)
		{
			cube.transform.position =
				mainCamera.transform.position +
				mainCamera.transform.Forward() * 3.0f +
				mainCamera.transform.Right() * 2.0f +
				mainCamera.transform.Up() * -1.0f;

			cube.transform.rotation = mainCamera.transform.rotation;
			ikframe.transform.position = cube.transform.position + cube.transform.Forward();
			ikframe.transform.rotation = cube.transform.rotation;
		}
		else
		{
			ikframe.transform.position = cube.transform.position + cube.transform.Forward();
			ikframe.transform.rotation = cube.transform.rotation;
		}

		goalmarker.transform.position = ikframe.goal;
		sphere.transform.position = ikframe.transform.position;

		ikframe.UpdateIK();
		std::cout << "reachable? " << ikframe.goalReachable << std::endl;



		// ik trunk
		iktrunk.goal = spline.Sample(deviation);
		iktrunk.UpdateIK();



		// rendering

		glm::mat4 mvp; // more from this later
		glm::mat4 projection = mainCamera.GetProjectionMatrix();
		glm::mat4 view = mainCamera.GetViewMatrix();
		glm::mat4 pv = projection * view;

		int d_width, d_height;
		display.GetWindowSize(&d_width, &d_height);

		glViewport(0, 0, d_width, d_height);

		// prepare colour shader (for splines)
		colourShader.Use();
		colourShader.SetMat4("projectionview", pv);
		spline.Draw(&colourShader);

		// prepare outline shader
		invertedHullShader.Use();
		invertedHullShader.SetMat4("projectionview", pv);
		invertedHullShader.SetVec3("viewPos", mainCamera.transform.position);
		invertedHullShader.SetFloat("edgeThreshold", 0.3);
		invertedHullShader.SetFloat("thicknessMin", thicknessMin);
		invertedHullShader.SetFloat("thicknessMax", thicknessMax);

		// prepare flat shader
		flatShader.Use();
		flatShader.SetMat4("projectionview", pv);
		flatShader.SetVec3("viewPos", mainCamera.transform.position);
		flatShader.SetVec3("light.position", mainCamera.transform.position);
		flatShader.SetVec3("light.colour", glm::vec3(1.0));
		flatShader.SetVec3("material.diffuse", glm::vec3(0.8));
		flatShader.SetVec3("material.specular", glm::vec3(0.99));
		flatShader.SetFloat("material.shininess", 32.0);
		flatShader.SetInt("numShades", 3);


		// Inverted hull stuff
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CW);

		invertedHullShader.Use();

		cube.Draw(&invertedHullShader);
		sphere.Draw(&invertedHullShader);
		goalmarker.Draw(&invertedHullShader);

		ikframe.Draw(&invertedHullShader);
		iktrunk.Draw(&invertedHullShader);


		glFrontFace(GL_CCW);
		
		
		flatShader.Use();

		cube.Draw(&flatShader);
		sphere.Draw(&flatShader);
		goalmarker.Draw(&flatShader);

		ikframe.Draw(&flatShader);
		iktrunk.Draw(&flatShader);


		//greenSkybox.Draw(&skyboxShader, &mainCamera);

		glBindVertexArray(0);
		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
