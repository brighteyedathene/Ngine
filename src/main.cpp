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
	lightPyramid.transform.position = glm::vec3(0.0f, 1.5f, -1.0f);


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
	missile.transform.scale = glm::vec3(0.1f);
	missile.transform.position.y = 1.0f;

	missile.material.ambient = glm::vec3(0.2f, 0.2f, 0.3f);
	missile.material.diffuse = glm::vec3(0.4f, 0.45f, 0.6f);
	missile.material.specular = glm::vec3(0.8, 0.8f, 0.8f);
	missile.material.shininess = 12.0f;


	// Skull object
	Model skullModel(skullPath);
	NaiveGameObject skull;
	skull.SetMesh(&skullModel);
	skull.SetInput(&input);
	skull.transform.scale = glm::vec3(0.5f);
	skull.transform.position.y = 2.0f;

	skull.material.ambient = glm::vec3(0.2f, 0.15f, 0.1f);
	skull.material.diffuse = glm::vec3(0.9f, 0.7f, 0.5f);
	skull.material.specular = glm::vec3(0.8, 0.8f, 0.8f);
	skull.material.shininess = 12.0f;


#pragma region camera_init

	Camera mainCamera;
	mainCamera.transform.position.y = 4.0f;
	mainCamera.farClipDistance = 1000;
	CameraController camController(&mainCamera, &input);
	bool freezecam = false; // TODO move this to camera class						
	SDL_SetRelativeMouseMode(SDL_TRUE);

	// set up camera for shader showcase
	mainCamera.transform.rotation = glm::vec3(-25, 148, 0.0);
	mainCamera.transform.position = glm::vec3(1.5, 1.8, -0.9);

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


	// Shader switches
	input.CreateButtonMapping("shininess+", SDL_SCANCODE_3);
	input.CreateButtonMapping("shininess-", SDL_SCANCODE_4);
	input.CreateButtonMapping("numShades+", SDL_SCANCODE_1);
	input.CreateButtonMapping("numShades-", SDL_SCANCODE_2);
	input.CreateButtonMapping("specularCoefficient+", SDL_SCANCODE_5);
	input.CreateButtonMapping("specularCoefficient-", SDL_SCANCODE_6);
	input.CreateButtonMapping("darkenCoefficient+", SDL_SCANCODE_7);
	input.CreateButtonMapping("darkenCoefficient-", SDL_SCANCODE_8);

	int param_numShades = 2;
	float param_shininess = 32.0;
	float param_specularCoefficient = 1.0;
	float param_darkenCoefficient = 1.0;

	// Toggle automatic movement for showcase
	input.CreateButtonMapping("toggleMovingLight", SDL_SCANCODE_T);
	input.CreateButtonMapping("toggleMovingModel", SDL_SCANCODE_R);
	bool movingLight = false;
	bool movingModel = true;


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
			missile.transform.position += missile.transform.Forward() * 0.05f;
		if (input.GetButton("b_Backward"))
			missile.transform.position -= missile.transform.Forward() * 0.05f;
		if (input.GetButton("b_YawLeft"))
			missile.transform.rotation.y -= 5.0f;
		if (input.GetButton("b_YawRight"))
			missile.transform.rotation.y += 5.0f;
		if (input.GetButton("b_PitchUp"))
			missile.transform.rotation.x -= 5.0f;
		if (input.GetButton("b_PitchDown"))
			missile.transform.rotation.x += 5.0f;
		if (input.GetButton("b_RollLeft"))
			missile.transform.rotation.z -= 5.0f;
		if (input.GetButton("b_RollRight"))
			missile.transform.rotation.z += 5.0f;


		// Update shader parameters
		// flat shader shades
		if (input.GetButtonDown("numShades+")) {
			param_numShades += 1;
			std::cout << "NumShades: " << param_numShades << std::endl;
		}
		if (input.GetButtonDown("numShades-")) {
			param_numShades = max(1, param_numShades - 1);
			std::cout << "NumShades: " << param_numShades << std::endl;
		}
		// shininess exponent
		if (input.GetButtonDown("shininess+")) {
			param_shininess *= 2;
			std::cout << "Shininess: " << param_shininess << std::endl;
		}
		if (input.GetButtonDown("shininess-")) {
			param_shininess /= 2;
			std::cout << "Shininess: " << param_shininess << std::endl;
		}
		// material's specular component
		if (input.GetButtonDown("specularCoefficient+")) {
			param_specularCoefficient *= 1.2;
			std::cout << "SpecularCoefficient: " << param_specularCoefficient << std::endl;
		}
		if (input.GetButtonDown("specularCoefficient-")) {
			param_specularCoefficient *= 0.8;
			std::cout << "SpecularCoefficient: " << param_specularCoefficient << std::endl;
		}
		// minneart darken coefficient
		if (input.GetButtonDown("darkenCoefficient+")) {
			param_darkenCoefficient *= 1.2;
			std::cout << "darkenCoefficient: " << param_darkenCoefficient << std::endl;
		}
		if (input.GetButtonDown("darkenCoefficient-")) {
			param_darkenCoefficient *= 0.8;
			std::cout << "darkenCoefficient: " << param_darkenCoefficient << std::endl;
		}

		// Toggle showcase movement
		if (input.GetButtonDown("toggleMovingLight"))
			movingLight = !movingLight;
		if (input.GetButtonDown("toggleMovingModel"))
			movingModel = !movingModel;

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


		// Move the light pyramid around
		if (movingLight) {
			lightPyramid.transform.rotation.y += 360* gameclock.deltaTime;
			lightPyramid.transform.position.y = 1 + sin(time)/2;
			lightPyramid.transform.position.x = 1.5*sin(time);
			lightPyramid.transform.position.z = 1.5*cos(time);
		}
		
		// Rotate the models
		if (movingModel) {
			skull.transform.rotation.y += gameclock.deltaTime*2;
			missile.transform.rotation.y += gameclock.deltaTime*2;
		}


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
		/*-------------------------------------------------------------------*/
		/*-------------------------- Blinn/Phong ----------------------------*/
		/*-------------------------------------------------------------------*/
		glViewport(0, 0, (d_width/3) -1, d_height);

		// Draw the light
		lightShader.Use();
		glBindVertexArray(pyrVAO);
		mvp = pv * lightPyramid.transform.GetMatrix();
		lightShader.SetMat4("mvp", mvp);
		lightShader.SetVec3("ucolour", lightPyramid.colour);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);


		// Now draw the lit objects
		blinnPhongShader.Use();

		blinnPhongShader.SetVec3("light.position", lightPyramid.transform.position);
		blinnPhongShader.SetVec3("light.colour",  lightPyramid.colour);
		blinnPhongShader.SetVec3("viewPos", mainCamera.transform.position);
		blinnPhongShader.SetMat4("projectionview", pv);

		// floor
		floor.Draw(&blinnPhongShader);

		// missile
		missile.material.specular = glm::vec3(param_specularCoefficient);
		missile.material.shininess = param_shininess;
		missile.Draw(&blinnPhongShader);

		// skull
		skull.material.specular = glm::vec3(param_specularCoefficient);
		skull.material.shininess = param_shininess;
		skull.Draw(&blinnPhongShader);

		/*-------------------------------------------------------------------*/
		/*------------------------- Flat Shader -----------------------------*/
		/*-------------------------------------------------------------------*/
		glViewport(d_width/3, 0, (d_width / 3) - 1, d_height);

		// Draw the light
		lightShader.Use();
		glBindVertexArray(pyrVAO);
		mvp = pv * lightPyramid.transform.GetMatrix();
		lightShader.SetMat4("mvp", mvp);
		lightShader.SetVec3("ucolour", lightPyramid.colour);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);


		// Now draw the lit objects
		flatShader.Use();

		flatShader.SetVec3("light.position", lightPyramid.transform.position);
		flatShader.SetVec3("light.colour", lightPyramid.colour);
		flatShader.SetVec3("viewPos", mainCamera.transform.position);
		flatShader.SetMat4("projectionview", pv);

		// set the shader-specific parameters
		flatShader.SetInt("numShades", param_numShades);

		// floor
		floor.Draw(&flatShader);

		// missile
		missile.material.specular = glm::vec3(param_specularCoefficient);
		missile.material.shininess = param_shininess;
		missile.Draw(&flatShader);

		// skull
		skull.material.specular = glm::vec3(param_specularCoefficient);
		skull.material.shininess = param_shininess;
		skull.Draw(&flatShader);

		/*-------------------------------------------------------------------*/
		/*----------------------- Minnaert Shader ---------------------------*/
		/*-------------------------------------------------------------------*/
		glViewport(2* d_width / 3, 0, (d_width / 3) - 1, d_height);

		// Draw the light
		lightShader.Use();
		glBindVertexArray(pyrVAO);
		mvp = pv * lightPyramid.transform.GetMatrix();
		lightShader.SetMat4("mvp", mvp);
		glDrawElements(GL_TRIANGLES, pyrCount, GL_UNSIGNED_INT, 0);

		// Now draw the lit objects
		minnaertShader.Use();

		minnaertShader.SetVec3("light.position", lightPyramid.transform.position);
		minnaertShader.SetVec3("light.colour", lightPyramid.colour);
		minnaertShader.SetVec3("viewPos", mainCamera.transform.position);
		minnaertShader.SetMat4("projectionview", pv);

		// set the shader-specific parameters
		minnaertShader.SetFloat("darkenCoefficient", param_darkenCoefficient);

		// floor
		floor.Draw(&minnaertShader);

		// missile
		missile.material.specular = glm::vec3(param_specularCoefficient);
		missile.material.shininess = param_shininess;
		missile.Draw(&minnaertShader);

		// skull
		skull.material.specular = glm::vec3(param_specularCoefficient);
		skull.material.shininess = param_shininess;
		skull.Draw(&minnaertShader);

		glBindVertexArray(0);
		display.Update();
	}

	// delete the Display shit
	display.~Display();
	return 0;
}
