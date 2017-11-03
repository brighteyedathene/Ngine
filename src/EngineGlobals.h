#pragma once
#ifndef ENGINEGLOBALS_H 
#define ENGINEGLOBALS_H


#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ngine
{
	int WINDOW_WIDTH = 900; // initial width
	int WINDOW_HEIGHT = 600; // initial height

	const GLchar* vertexShaderPath = "./Shaders/shader.vs";
	const GLchar* fragmentShaderPath = "./Shaders/shader.fs";

	const GLchar* lightingTestVertexShaderPath = "./Shaders/lightingTestShader.vs";
	const GLchar* lightingTestFragmentShaderPath = "./Shaders/lightingtestShader.fs";

	const GLchar* lightVertexShaderPath = "./Shaders/lightShader.vs";
	const GLchar* lightFragmentShaderPath = "./Shaders/lightShader.fs";

	const GLchar* texturePath = "./Textures/myTexture.jpg";
	const GLchar* otherTexturePath = "./Textures/otherTexture.jpg";
	const GLchar* eyePath = "./Textures/eye.jpg";

	const char* bearPath = "./Models/bear/bear.fbx";
	const char* bninjaPath = "./Models/bninja/bnin_collada.dae";

	const char* cubeModelPath = "./Models/cube/cube.obj";
	//const char* modelPath = "./Models/nanosuit/nanosuit.obj";
	//const char* modelPath = "./Models/ybot.fbx";

	
}

#endif