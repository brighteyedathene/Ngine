#pragma once
#ifndef ENGINEGLOBALS_H 
#define ENGINEGLOBALS_H

#include <vector>

#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ngine
{
	int WINDOW_WIDTH = 1280; // initial width
	int WINDOW_HEIGHT = 720; // initial height


#pragma region shaders

	const GLchar* vertexShaderPath = "./Shaders/shader.vs";
	const GLchar* fragmentShaderPath = "./Shaders/shader.fs";

	const GLchar* lightingTestVertexShaderPath = "./Shaders/lightingTestShader.vs";
	const GLchar* lightingTestFragmentShaderPath = "./Shaders/lightingTestShader.fs";

	const GLchar* animVertexShaderPath = "./Shaders/animShader.vs";
	const GLchar* animFragmentShaderPath = "./Shaders/animShader.fs";

	const GLchar* flatShaderVertexPath = "./Shaders/flatShader.vs";
	const GLchar* flatShaderFragmentPath = "./Shaders/flatShader.fs";

	const GLchar* blinnPhongShaderVertexPath = "./Shaders/blinnPhong.vs";
	const GLchar* blinPhongShaderFragmentPath = "./Shaders/blinnPhong.fs";

	const GLchar* minnaertShaderVertexPath = "./Shaders/minnaertShader.vs";
	const GLchar* minnaertShaderFragmentPath = "./Shaders/minnaertShader.fs";

	const GLchar* lightVertexShaderPath = "./Shaders/lightShader.vs";
	const GLchar* lightFragmentShaderPath = "./Shaders/lightShader.fs";

	const GLchar* skyboxFragmentPath = "./Shaders/skyboxShader.fs";
	const GLchar* skyboxVertexPath = "./Shaders/skyboxShader.vs";
	
	const GLchar* transmittanceFragmentPath = "./Shaders/transmittanceShader.fs";
	const GLchar* transmittanceVertexPath = "./Shaders/transmittanceShader.vs";

#pragma endregion shaders


	const GLchar* texturePath = "./Textures/myTexture.jpg";
	const GLchar* otherTexturePath = "./Textures/otherTexture.jpg";
	const GLchar* eyePath = "./Textures/eye.jpg";

	const char* bearPath = "./Models/bear/bear.fbx";
	//const char* bearPath = "./Models/bear/mayabear.obj";
	const char* missilePath = "./Models/missile/missile.obj";
	const char* skullPath = "./Models/skull/skull.obj"; // obtained from https://sketchfab.com/models/1a9db900738d44298b0bc59f68123393#
	const char* bninjaPath = "./Models/bninja/bnin_Idle_Textured.dae";

	const char* majorkongPath = "./Models/majorkong/majorkong-only.dae";
	//const char* majorkongPath = "./Models/majorkong/idle-test.dae";
	//const char* majorkongPath = "./Models/majorkong/kong-nobomb.fbx";
	
	const char* cubeModelPath = "./Models/cube/cube.obj";
	const char* sphereModelPath = "./Models/sphere/sphere.obj";

	const char* mixamoFBXPATH = "./Models/shit-for-testing/ybot_swagger_withmodel.fbx";
	
	// obtained this cubemap from http://www.custommapmakers.org/skyboxes.php
	std::string greenCubemapFaces_array[] =
	{


		"./Textures/cubemap_green/green_ft.tga",
		"./Textures/cubemap_green/green_bk.tga",
		"./Textures/cubemap_green/green_up.tga",
		"./Textures/cubemap_green/green_dn.tga",
		"./Textures/cubemap_green/green_rt.tga",
		"./Textures/cubemap_green/green_lf.tga",
	};
	std::vector<std::string> greenCubemapFaces(greenCubemapFaces_array, greenCubemapFaces_array + sizeof(greenCubemapFaces_array) / sizeof(greenCubemapFaces_array[0]));
	
	std::string exampleSkyboxFaces_array[] =
	{
		"./Textures/example_skybox/right.jpg",
		"./Textures/example_skybox/left.jpg",
		"./Textures/example_skybox/bottom.jpg",
		"./Textures/example_skybox/top.jpg",
		"./Textures/example_skybox/front.jpg",
		"./Textures/example_skybox/back.jpg",
	};
	//std::vector<std::string> greenCubemapFaces(exampleSkyboxFaces_array, exampleSkyboxFaces_array + sizeof(exampleSkyboxFaces_array) / sizeof(exampleSkyboxFaces_array[0]));
}

#endif