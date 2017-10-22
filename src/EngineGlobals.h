#pragma once
#ifndef ENGINEGLOBALS_H 
#define ENGINEGLOBALS_H


#include <SDL2/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace ngine
{
	int WINDOW_WIDTH = 900; // initial width
	int WINDOW_HEIGHT = 601; // initial height

	const GLchar* vertexShaderPath = "./Shaders/shader.vs";
	const GLchar* fragmentShaderPath = "./Shaders/shader.fs";

	const GLchar* texturePath = "./Textures/myTexture.jpg";
	const GLchar* otherTexturePath = "./Textures/otherTexture.jpg";
	const GLchar* eyePath = "./Textures/eye.jpg";

	const char* modelPath = "./Models/nanosuit/nanosuit.obj";
	//const char* modelPath = "./Models/ybot.fbx";

	struct Clock
	{

		double deltaTime;
		Uint64 NOW = 0;
		Uint64 LAST = 0;
		
		void Tick()
		{
			LAST = NOW;
			NOW = SDL_GetPerformanceCounter();
			deltaTime = (double)(NOW - LAST) / (double)SDL_GetPerformanceFrequency();
		}
	};
}

#endif